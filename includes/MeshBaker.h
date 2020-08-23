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

#define METHOD_DIFFUSE  0

struct LightmapBakerParams
{
    int method = 0;
    int geomIndex = 0;
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
        ResultSum = 0.0f;
    }

    vector SampleDirection(vector2 samplePoint)
    {
        return SampleCosineHemisphere(samplePoint.x, samplePoint.y);
    }

    void AddSample(vector sampleDir; vector sample)
    {
        ResultSum += sample;
    }

    void FinalResult(vector bakeOutput[])
    {
        vector finalResult = ResultSum * CosineWeightedMonteCarloFactor(NumSamples);
        bakeOutput[0] = finalResult;
    }
};

int GetBasisCount(int method)
{
    if (method == METHOD_DIFFUSE)
        return 1;
    else
        return 0;
}

function vector[] LightmapBake(LightmapBakerParams params; BakePoint bakePoint)
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

    DiffuseBaker baker;

    vector texelResults[];
    resize(texelResults, baker.BasisCount);
    baker->Init(numSamplesPerTexel);

    matrix3 tangentToWorld = matrix3(set(bakePoint.tangent, bakePoint.bitangent, bakePoint.normal));

    for (int sampleIdx = 0; sampleIdx < numSamplesPerTexel; ++sampleIdx)
    {
        vector rayStart = bakePoint.position;
        vector rayDirTS = baker->SampleDirection(set(nrandom(), nrandom()));
        vector rayDirWS = vtransform(rayDirTS, tangentToWorld);

        Ray ray;
        ray.direction = rayDirWS;
        ray.origin = rayStart + rayDirWS * params.rayPosNormalNudge;

        vector sampleResult = PathTrace(pathTracerParams, ray);

        baker->AddSample(ray.direction, sampleResult);
    }

    baker->FinalResult(texelResults);

    return texelResults;
}