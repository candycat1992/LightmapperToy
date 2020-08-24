#include "$HIP/includes/PathTracer.h"

// =================================================
// Helpers
// =================================================

// Returns the final monte-carlo weighting factor using the PDF of a cosine-weighted hemisphere
function float CosineWeightedMonteCarloFactor(int numSamples)
{
    // Integrating cosine factor about the hemisphere gives you Pi, and the PDF
    // of a cosine-weighted hemisphere function is 1 / Pi.
    // So the final monte-carlo weighting factor is 1 / NumSamples
    return (1.0f / numSamples);
}

function float HemisphereMonteCarloFactor(int numSamples)
{
    // The area of a unit hemisphere is 2 * Pi, so the PDF is 1 / (2 * Pi)
    return ((2.0f * Pi) / numSamples);
}

function float SphereMonteCarloFactor(int numSamples)
{
    // The area of a unit hemisphere is 2 * Pi, so the PDF is 1 / (2 * Pi)
    return ((4.0f * Pi) / numSamples);
}

// =================================================
// Lightmap Baking
// =================================================

#define METHOD_DIFFUSE      0
#define METHOD_DIRECTIONAL  1

struct LightmapBakerParams
{
    int geomIndex = 0;
    int method = 0;
    int seed = 0;
    int enableDiffuse = 1;
    int enableSpecular = 1;
    int enableAlbedo = 1;
    int sqrtNumSamples = 16;
    int maxBounces = 8;
    float rayPosNormalNudge = 0.001;
    float rayMinDistance = 0.001;
    float rayMaxDistance = 10000.0;
    int russianRouletteDepth = -1;
    float russianRouletteProbability = 0.5f;
};

// Info about a single point on the light map that needs to be baked
struct BakePoint
{
    vector position;
    vector normal;
    vector tangent;
    vector bitangent;
};

// Bakes irradiance / Pi as 3 floats per texel
struct DiffuseBaker
{
    int BasisCount = 1;

    int NumSamples = 0;
    vector ResultSum;

    void Init(int numSamples)
    {
        NumSamples = numSamples;
        ResultSum = 0.0;
    }

    vector SampleDirection(vector2 samplePoint)
    {
        return SampleCosineHemisphere(samplePoint.x, samplePoint.y);
    }

    void AddSample(int sampleIndex; vector sample; vector sampleDirTS; vector sampleDirWS; vector normal)
    {
        ResultSum += sample;
    }

    void FinalResult(vector4 bakeOutput[])
    {
        vector finalResult = ResultSum * CosineWeightedMonteCarloFactor(NumSamples);
        bakeOutput[0] = set(finalResult.x, finalResult.y, finalResult.z, 1.0);
    }
};

// Bakes irradiance based on Enlighten's directional approach, with 3 floats for color,
// 3 for lighting main direction information, and 1 float to ensure that the directional
// term evaluates to 1 when the surface normal aligns with normal used when baking.
//
// NOTE: A directional map can be encoded per RGB channel which puts the memory cost at
// the cost of L1 SH but at a worse quality. This implementation with a single directional
// map is provided as a cheap alternative at the expense of quality.
//
// Reference: https://static.docs.arm.com/100837/0308/enlighten_3-08_sdk_documentation__100837_0308_00_en.pdf
struct DirectionalBaker
{
    int BasisCount = 2;

    int NumSamples = 0;
    vector ResultSum;
    vector DirectionSum;
    float DirectionWeightSum;
    vector NormalSum;

    void Init(int numSamples)
    {
        NumSamples = numSamples;
        ResultSum = 0.0;
        DirectionSum = 0.0;
        DirectionWeightSum = 0.0;
        NormalSum = 0.0;
    }

    vector SampleDirection(vector2 samplePoint)
    {
        return SampleCosineHemisphere(samplePoint.x, samplePoint.y);
    }

    void AddSample(int sampleIndex; vector sample; vector sampleDirTS; vector sampleDirWS; vector normal)
    {
        normal = normalize(normal);

        vector sampleDir = normalize(sampleDirWS);

        ResultSum += sample;
        DirectionSum += sampleDir * luminance(sample);
        DirectionWeightSum += luminance(sample);
        NormalSum += normal;
    }

    void FinalResult(vector4 bakeOutput[])
    {
        vector finalResult = ResultSum * CosineWeightedMonteCarloFactor(NumSamples);
        bakeOutput[0] = finalResult;

        vector finalColorResult = ResultSum * CosineWeightedMonteCarloFactor(NumSamples);

        vector finalDirection = normalize(DirectionSum / max(DirectionWeightSum, 0.0001));

        vector averageNormal = normalize(NormalSum * CosineWeightedMonteCarloFactor(NumSamples));
        vector4 tau = set(averageNormal.x, averageNormal.y, averageNormal.z, 1.0f) * 0.5f;

        bakeOutput[0] = set(finalColorResult.x, finalColorResult.y, finalColorResult.z, 1.0f);
        bakeOutput[1] = set(finalDirection.x * 0.5f + 0.5, finalDirection.y * 0.5f + 0.5, finalDirection.z * 0.5f + 0.5, max(dot(tau, set(finalDirection.x, finalDirection.y, finalDirection.z, 1.0f)), 0.0001));
    }
};

function vector4[] LightmapBake(LightmapBakerParams params; BakePoint bakePoint)
{
    PathTracerParams pathTracerParams;
    pathTracerParams.geomIndex = params.geomIndex;
    pathTracerParams.seed = params.seed;
    pathTracerParams.enableDiffuse = params.enableDiffuse;
    pathTracerParams.enableSpecular = params.enableSpecular;
    pathTracerParams.enableAlbedo = params.enableAlbedo;
    pathTracerParams.maxBounces = params.maxBounces;
    pathTracerParams.rayPosNormalNudge = params.rayPosNormalNudge;
    pathTracerParams.rayMinDistance = params.rayMinDistance;
    pathTracerParams.rayMaxDistance = params.rayMaxDistance;
    pathTracerParams.russianRouletteDepth = params.russianRouletteDepth;
    pathTracerParams.russianRouletteProbability = params.russianRouletteProbability;

    int numSamplesPerTexel = params.sqrtNumSamples * params.sqrtNumSamples;

    matrix3 tangentToWorld = matrix3(set(bakePoint.tangent, bakePoint.bitangent, bakePoint.normal));

    vector4 texelResults[];
    if (params.method == METHOD_DIFFUSE)
    {
        DiffuseBaker baker;
        resize(texelResults, baker.BasisCount);
        baker->Init(numSamplesPerTexel);

        for (int sampleIdx = 0; sampleIdx < numSamplesPerTexel; ++sampleIdx)
        {
            vector rayStart = bakePoint.position;
            vector rayDirTS = baker->SampleDirection(set(nrandom(), nrandom()));
            vector rayDirWS = vtransform(rayDirTS, tangentToWorld);

            Ray ray;
            ray.direction = rayDirWS;
            ray.origin = rayStart + rayDirWS * params.rayPosNormalNudge;

            vector sampleResult = PathTrace(pathTracerParams, ray);

            baker->AddSample(sampleIdx, sampleResult, rayDirTS, rayDirWS, bakePoint.normal);
        }
        baker->FinalResult(texelResults);
    }
    else if (params.method == METHOD_DIRECTIONAL)
    {
        DirectionalBaker baker;
        resize(texelResults, baker.BasisCount);
        baker->Init(numSamplesPerTexel);

        for (int sampleIdx = 0; sampleIdx < numSamplesPerTexel; ++sampleIdx)
        {
            vector rayStart = bakePoint.position;
            vector rayDirTS = baker->SampleDirection(set(nrandom(), nrandom()));
            vector rayDirWS = vtransform(rayDirTS, tangentToWorld);

            Ray ray;
            ray.direction = rayDirWS;
            ray.origin = rayStart + rayDirWS * params.rayPosNormalNudge;

            vector sampleResult = PathTrace(pathTracerParams, ray);

            baker->AddSample(sampleIdx, sampleResult, rayDirTS, rayDirWS, bakePoint.normal);
        }
        
        baker->FinalResult(texelResults);
    }

    return texelResults;
}