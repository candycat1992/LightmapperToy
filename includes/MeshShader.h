#include "$HIP/includes/MeshBaker.h"

struct MeshShaderParams
{
    int geomIndex = 0;
    int method = 0;
    int enableDiffuse = 1;
    int enableSpecular = 1;
    int enableAlbedo = 1;
};

function vector MeshShade(MeshShaderParams params; MaterialInfo material; vector lightmap[])
{
    vector lighting = {0, 0, 0};

    // Override material properties
    material.basecolor = params.enableAlbedo > 0 ? material.basecolor : {1, 1, 1};

    vector diffuseColor = lerp(material.basecolor, {0.0, 0.0, 0.0}, material.metallic);
    vector specularColor = lerp({0.02, 0.02, 0.02}, material.basecolor, material.metallic);

    // Add in the emissive lighting
    lighting += material.emissive;

    // Add in the indirect lighting
    {
        vector indirectIrradiance = {0, 0, 0};
        vector indirectSpecular = {0, 0, 0};

        if (params.method == METHOD_DIFFUSE)
        {
            indirectIrradiance = lightmap[0];
        }

        lighting += indirectIrradiance * (diffuseColor / PI);
    }

    return lighting;
}