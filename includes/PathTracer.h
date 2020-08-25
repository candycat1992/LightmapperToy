#ifndef PATHTRACER_INCLUDED
#define PATHTRACER_INCLUDED 1

#include "$HIP/includes/Sampling.h"
#include "$HIP/includes/BRDF.h"

#define MAT_TYPE_LIGHT      0
#define MAT_TYPE_STATIC     1
#define MAT_TYPE_MOVABLE    2

// =================================================
// Helpers
// =================================================

function vector SampleEnvironment(vector rayDir)
{
    matrix envTrans = maketransform(0, 0, {0, 0, 0}, set(0, -chf(chs("../env_light_path") + "/ry"), 0));
    return environment(ch(chs("../env_light_path") + "/env_map"), rayDir * envTrans);
}

// =================================================
// Ray tracing
// =================================================

struct PathTracerParams
{
    int geomIndex = 0;
    int seed = 0;
    int enableDiffuse = 1;
    int enableSpecular = 1;
    int enableAlbedo = 1;
    int maxBounces = 8;
    float rayPosNormalNudge = 0.001;
    float rayMinDistance = 0.001;
    float rayMaxDistance = 10000.0;
    int russianRouletteDepth = -1;
    float russianRouletteProbability = 0.5f;
};

struct Ray
{
    vector origin;
    vector direction;
};

struct MaterialInfo
{
    vector2 texcoord;
    vector basecolor;
    float specular;
    float metallic;
    float roughness;
    vector emissive;
};

struct RayHitInfo
{
    int hitten;
    int prim;
    vector uvw;
    vector pos;
    vector normal;
    vector tangent;
    vector bitangent;
    MaterialInfo material;
};

function RayHitInfo RayIntersect(int geomIndex; float rayMinDistance; float rayMaxDistance; Ray ray)
{
    RayHitInfo hitInfo;

    vector hitPos, hitUVW;
    int hitPrim = intersect(geomIndex, ray.origin + ray.direction * rayMinDistance, ray.direction * rayMaxDistance, hitPos, hitUVW);

    if (hitPrim < 0)
    {
        hitInfo.hitten = -1;
    }
    else
    {
        hitInfo.hitten = 1;
        hitInfo.prim = hitPrim;
        hitInfo.uvw = hitUVW;
        hitInfo.pos = hitPos;
        hitInfo.normal = primuv(geomIndex, 'N', hitPrim, hitUVW);
        hitInfo.tangent = primuv(geomIndex, 'tangentu', hitPrim, hitUVW);
        hitInfo.bitangent = primuv(geomIndex, 'tangentv', hitPrim, hitUVW);

        hitInfo.material.texcoord   = primuv(geomIndex, 'uv', hitPrim, hitUVW);
        hitInfo.material.basecolor  = primuv(geomIndex, 'basecolor', hitPrim, hitUVW);
        hitInfo.material.specular   = primuv(geomIndex, 'specular', hitPrim, hitUVW);
        hitInfo.material.metallic   = primuv(geomIndex, 'metallic', hitPrim, hitUVW);
        hitInfo.material.roughness  = primuv(geomIndex, 'roughness', hitPrim, hitUVW);
        hitInfo.material.emissive   = primuv(geomIndex, 'emissive', hitPrim, hitUVW);
    }

    return hitInfo;
}

function vector PathTrace(PathTracerParams params; Ray ray)
{
    vector radiance = {0.0, 0.0, 0.0};
    vector irradiance = {0.0, 0.0, 0.0};
    vector throughput = {1.0, 1.0, 1.0};
    vector irrThroughput = {1.0, 1.0, 1.0};

    int maxPathLength = params.maxBounces;

    // Keep tracing paths until we reach the specified max
    Ray curRay = ray;
    for (int pathLength = 1; pathLength <= maxPathLength || maxPathLength == -1; ++pathLength)
    {
        // See if we should randomly terminate this path using Russian Roullete
        int rouletteDepth = params.russianRouletteDepth;
        if (pathLength >= rouletteDepth && rouletteDepth != -1)
        {
            float continueProbability = min(params.russianRouletteProbability, max(throughput.x, max(throughput.y, throughput.z)));
            if (nrandom() > continueProbability)
                break;
            throughput *= 1.0 / continueProbability;
            irrThroughput *= 1.0 / continueProbability;
        }

        // Set this to true to keep the loop going
        int continueTracing = -1;

        // Check for intersection with the scene
        RayHitInfo hitInfo = RayIntersect(params.geomIndex, params.rayMinDistance, params.rayMaxDistance, curRay);

        if (hitInfo.hitten < 0) // We hit the sky
        {
            // We sample the sky radiance and then bail out
            vector cubeMapRadiance = SampleEnvironment(curRay.direction);
            radiance += cubeMapRadiance * throughput;
            irradiance += cubeMapRadiance * irrThroughput;
        }
        else // We hit a triangle in the scene
        {
            radiance += hitInfo.material.emissive * throughput;
            irradiance += hitInfo.material.emissive * irrThroughput;

            if (pathLength == maxPathLength)
            {
                // There's no point in continuing anymore
                break;
            }

            // Override material properties
            hitInfo.material.basecolor = params.enableAlbedo > 0 ? hitInfo.material.basecolor : {1, 1, 1};

            vector diffuseColor = lerp(hitInfo.material.basecolor, {0.0, 0.0, 0.0}, hitInfo.material.metallic);
            vector specularColor = lerp({0.02, 0.02, 0.02}, hitInfo.material.basecolor, hitInfo.material.metallic);

            matrix3 tangentToWorld = matrix3(set(hitInfo.tangent, hitInfo.bitangent, hitInfo.normal));

            int enableDiffuseSampling = (params.enableDiffuse && hitInfo.material.metallic < 1.0) ? 1 : -1;
            int enableSpecularSampling = (params.enableSpecular && pathLength == 1) ? 1 : -1;

            if (enableDiffuseSampling || enableSpecularSampling)
            {
                vector2 brdfSample = set(nrandom(), nrandom());
                float selector = nrandom();
                if (enableSpecularSampling < 0)
                    selector = 0.0;
                else if (enableDiffuseSampling <0)
                    selector = 1.0;

                vector sampleDir;
                vector v = normalize(curRay.origin - hitInfo.pos);

                if (selector < 0.5)
                {
                    // We're sampling the diffuse BRDF, so sample a cosine-weighted hemisphere
                    sampleDir = SampleCosineHemisphere(brdfSample.x, brdfSample.y);
                    sampleDir = vtransform(sampleDir, tangentToWorld);
                }
                else
                {
                    // We're sampling the GGX specular BRDF
                    sampleDir = SampleDirectionGGX(v, hitInfo.normal, hitInfo.material.roughness, tangentToWorld, brdfSample.x, brdfSample.y);
                }

                vector h = normalize(v + sampleDir);
                float nDotL = clamp(dot(sampleDir, hitInfo.normal), 0.0f, 1.0f);

                float diffusePDF = (enableDiffuseSampling > 0) ? nDotL / PI : 0.0f;
                float specularPDF = (enableSpecularSampling > 0) ? GGX_PDF(hitInfo.normal, h, v, hitInfo.material.roughness) : 0.0f;
                float pdf = diffusePDF + specularPDF;
                if ((enableDiffuseSampling > 0) && (enableSpecularSampling > 0))
                    pdf *= 0.5;

                if (nDotL > 0.0f && pdf > 0.0f && dot(sampleDir, hitInfo.normal) > 0.0f)
                {
                    // Compute both BRDF's
                    vector brdf = 0.0;

                    if (enableDiffuseSampling > 0)
                    {
                        brdf += diffuseColor / PI;
                    }
                    if (enableSpecularSampling > 0)
                    {
                        float spec = GGX_Specular(hitInfo.material.roughness, hitInfo.normal, h, v, sampleDir);
                        brdf += Fresnel(specularColor, h, sampleDir) * spec;
                    }

                    throughput *= brdf * nDotL / pdf;
                    irrThroughput *= nDotL / pdf;

                    // Generate the ray for the new path
                    curRay.origin = hitInfo.pos + sampleDir * params.rayPosNormalNudge;
                    curRay.direction = sampleDir;

                    continueTracing = 1;
                }
            }
        }

        if (continueTracing < 0)
            break;
    }

    return radiance;
}

#endif // PATHTRACER_INCLUDED