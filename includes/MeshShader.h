#include "$HIP/includes/MeshBaker.h"

struct MeshShaderParams
{
    int geomIndex = 0;
    int method = 0;
    int enableDiffuse = 1;
    int enableSpecular = 1;
    int enableAlbedo = 1;
    int enableNormal = 1;
};

function vector MeshShade(MeshShaderParams params; MaterialInfo material; BakePoint bakePoint; vector4 lightmap[]; vector viewDir)
{
    vector lighting = {0, 0, 0};

    // Override material properties
    material.basecolor = params.enableAlbedo > 0 ? material.basecolor : {1, 1, 1};

    vector diffuseColor = lerp(material.basecolor, {0.0, 0.0, 0.0}, material.metallic) * (params.enableDiffuse > 0 ? 1.0 : 0.0);
    vector specularColor = lerp({0.02, 0.02, 0.02}, material.basecolor, material.metallic) * (params.enableSpecular > 0 ? 1.0 : 0.0);

    matrix3 tangentToWorld = matrix3(set(bakePoint.tangent, bakePoint.bitangent, bakePoint.normal));

    if (params.enableNormal > 0)
    {
        vector2 texcoord = material.texcoord * chf("../normal_map_tiling");
        vector normalDirTS = texture(chs("../normal_map_path"), texcoord.x, texcoord.y, "srccolorspace", "linear");
        normalDirTS = normalize(normalDirTS * 2.0 - 1.0);

        normalDirTS.x = normalDirTS.x * chf("../normal_map_scale");
        normalDirTS.y = normalDirTS.y * chf("../normal_map_scale");
		normalDirTS.z = sqrt(1.0 - clamp(normalDirTS.x * normalDirTS.x + normalDirTS.y * normalDirTS.y, 0.0, 1.0));

        bakePoint.normal = normalize(vtransform(normalDirTS, tangentToWorld));
    }

    // Add in the emissive lighting
    lighting += material.emissive;

    // Add in the indirect lighting
    {
        vector indirectIrradiance = {0, 0, 0};
        vector indirectSpecular = {0, 0, 0};

        if (params.method == METHOD_DIFFUSE)
        {
            indirectIrradiance = set(lightmap[0].x, lightmap[0].y, lightmap[0].z);
        }
        else if (params.method == METHOD_DIRECTIONAL)
        {
            vector lightMapColor = set(lightmap[0].x, lightmap[0].y, lightmap[0].z);
			vector lightmapDirection = set(lightmap[1].x, lightmap[1].y, lightmap[1].z);

			float rebalancingCoefficient = max(lightmap[1].w, 0.0001);

			lightmapDirection = lightmapDirection * 2.0 - 1.0;

			vector4 tau = set(bakePoint.normal.x, bakePoint.normal.y, bakePoint.normal.z, 1.0f) * 0.5f;
			float halfLambert = dot(tau, set(lightmapDirection.x, lightmapDirection.y, lightmapDirection.z, 1.0f));

            vector halfDir = normalize(viewDir + lightmapDirection);

			indirectIrradiance = lightMapColor * halfLambert / rebalancingCoefficient;
            indirectSpecular = lightMapColor * max(dot(bakePoint.normal, lightmapDirection), 0.0) * GGX_Specular(material.roughness, bakePoint.normal, halfDir, viewDir, lightmapDirection) * Fresnel(specularColor, halfDir, viewDir);
        }

        lighting += indirectIrradiance * (diffuseColor / PI);
        lighting += indirectSpecular;
    }

    return lighting;
}