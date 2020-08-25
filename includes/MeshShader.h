#ifndef MESHSHADER_INCLUDED
#define MESHSHADER_INCLUDED 1

#include "$HIP/includes/MeshBaker.h"

#define SH_DIFFUSE_MODE_CONVOLUTION     0
#define SH_DIFFUSE_MODE_GEOMERICS       1
#define SH_SPECULAR_MODE_CONVOLUTION    0
#define SH_SPECULAR_MODE_DOMINANT       1
#define SH_SPECULAR_MODE_PUNCTUAL       2
#define SH_SPECULAR_MODE_PREFILTERED    3

struct SHSettings
{
    int diffuseMode = 0;
    int specularMode = 0;
};

struct MeshShaderParams
{
    int geomIndex = 0;
    int method = 0;
    int enableDiffuse = 1;
    int enableSpecular = 1;
    int enableAlbedo = 1;

    SHSettings shSettings;
};

function vector MeshShade(MeshShaderParams params; MaterialInfo material; BakePoint bakePoint; vector4 lightmap[]; vector viewDir)
{
    vector lighting = {0, 0, 0};

    // Override material properties
    material.basecolor = params.enableAlbedo > 0 ? material.basecolor : {1, 1, 1};

    vector diffuseColor = lerp(material.basecolor, {0.0, 0.0, 0.0}, material.metallic) * (params.enableDiffuse > 0 ? 1.0 : 0.0);
    vector specularColor = lerp({0.02, 0.02, 0.02}, material.basecolor, material.metallic) * (params.enableSpecular > 0 ? 1.0 : 0.0);
    float sqrtRoughness = sqrt(material.roughness);

    matrix3 tangentToWorld = matrix3(set(bakePoint.tangent, bakePoint.bitangent, bakePoint.normal));
    vector normalWS = bakePoint.normal;
    vector normalTS = {0, 0, 1};
    
    // Add in the emissive lighting
    lighting += material.emissive;

    // Add in the indirect lighting
    {
        vector indirectIrradiance = {0, 0, 0};
        vector indirectSpecular = {0, 0, 0};

        if (params.method == METHOD_DIFFUSE)
        {
            indirectIrradiance = set(lightmap[0].x, lightmap[0].y, lightmap[0].z) * PI;
        }
        else if (params.method == METHOD_DIRECTIONAL)
        {
            vector lightMapColor = set(lightmap[0].x, lightmap[0].y, lightmap[0].z) * PI;
			vector lightmapDirection = set(lightmap[1].x, lightmap[1].y, lightmap[1].z);

			float rebalancingCoefficient = max(lightmap[1].w, 0.0001);

			lightmapDirection = lightmapDirection * 2.0 - 1.0;

			vector4 tau = set(bakePoint.normal.x, bakePoint.normal.y, bakePoint.normal.z, 1.0f) * 0.5f;
			float halfLambert = dot(tau, set(lightmapDirection.x, lightmapDirection.y, lightmapDirection.z, 1.0f));

            vector halfDir = normalize(viewDir + lightmapDirection);

			indirectIrradiance = lightMapColor * halfLambert / rebalancingCoefficient;
            indirectSpecular = lightMapColor * halfLambert / rebalancingCoefficient * GGX_Specular(material.roughness, bakePoint.normal, halfDir, viewDir, lightmapDirection) * Fresnel(specularColor, halfDir, viewDir);
        }
        else if (params.method == METHOD_HL2)
        {
            vector BasisDirs[] = array
            (
                set(-1.0f / sqrt(6.0f), -1.0f / sqrt(2.0f), 1.0f / sqrt(3.0f)),
                set(-1.0f / sqrt(6.0f), 1.0f / sqrt(2.0f), 1.0f / sqrt(3.0f)),
                set(sqrt(2.0f / 3.0f), 0.0f, 1.0f / sqrt(3.0f))
            );

            for (int i = 0; i < 3; ++i)
            {
                vector basis = set(lightmap[i].x, lightmap[i].y, lightmap[i].z);
                indirectIrradiance += clamp(dot(normalTS, BasisDirs[i]), 0.0f, 1.0f) * basis;
            }
        }
        else if (params.method == METHOD_SH4)
        {
            SH4Color shRadiance;

            for (int i = 0; i < 4; ++i)
                shRadiance.coefficients[i] = set(lightmap[i].x, lightmap[i].y, lightmap[i].z);

            if (params.shSettings.diffuseMode == SH_DIFFUSE_MODE_CONVOLUTION)
                indirectIrradiance = EvalSH4Irradiance(normalWS, shRadiance);
            else if (params.shSettings.diffuseMode == SH_DIFFUSE_MODE_GEOMERICS)
                indirectIrradiance = EvalSH4IrradianceGeomerics(normalWS, shRadiance);

            // if (params.shSettings.specularMode == SH_SPECULAR_MODE_CONVOLUTION)
            //     indirectSpecular = ConvolutionSHSpecular(viewDir, normalWS, specularColor, sqrtRoughness, ConvertToSH9(shRadiance));
            if (params.shSettings.specularMode == SH_SPECULAR_MODE_DOMINANT)
                 indirectSpecular = FrostbiteSHSpecular(viewDir, normalWS, specularColor, sqrtRoughness, shRadiance);
            else if (params.shSettings.specularMode == SH_SPECULAR_MODE_PUNCTUAL)
                indirectSpecular = PunctualSHSpecular(viewDir, normalWS, tangentToWorld, specularColor, sqrtRoughness, ConvertToSH9(shRadiance));
            // else if (params.shSettings.specularMode == SH_SPECULAR_MODE_PREFILTERED)
            //     indirectSpecular = PrefilteredSHSpecular(viewDir, normalWS, tangentToWorld, specularColor, sqrtRoughness, ConvertToSH9(shRadiance));
        }
        else if (params.method == METHOD_SH9)
        {
            SH9Color shRadiance;

            for (int i = 0; i < 9; ++i)
                shRadiance.coefficients[i] = set(lightmap[i].x, lightmap[i].y, lightmap[i].z);

            if (params.shSettings.diffuseMode == SH_DIFFUSE_MODE_CONVOLUTION)
                indirectIrradiance = EvalSH9Irradiance(normalWS, shRadiance);
            else if (params.shSettings.diffuseMode == SH_DIFFUSE_MODE_GEOMERICS)
                indirectIrradiance = EvalSH4IrradianceGeomerics(normalWS, ConvertToSH4(shRadiance));

            // if (params.shSettings.specularMode == SH_SPECULAR_MODE_CONVOLUTION)
            //     indirectSpecular = ConvolutionSHSpecular(viewDir, normalWS, specularColor, sqrtRoughness, ConvertToSH9(shRadiance));
            if (params.shSettings.specularMode == SH_SPECULAR_MODE_DOMINANT)
                 indirectSpecular = FrostbiteSHSpecular(viewDir, normalWS, specularColor, sqrtRoughness, ConvertToSH4(shRadiance));
            else if (params.shSettings.specularMode == SH_SPECULAR_MODE_PUNCTUAL)
                indirectSpecular = PunctualSHSpecular(viewDir, normalWS, tangentToWorld, specularColor, sqrtRoughness, shRadiance);
            // else if (params.shSettings.specularMode == SH_SPECULAR_MODE_PREFILTERED)
            //     indirectSpecular = PrefilteredSHSpecular(viewDir, normalWS, tangentToWorld, specularColor, sqrtRoughness, ConvertToSH9(shRadiance));
        }

        // Need to figure out!
        indirectIrradiance /= PI;

        lighting += indirectIrradiance * (diffuseColor / PI);
        lighting += indirectSpecular;
    }

    return lighting;
}

#endif // MESHSHADER_INCLUDED