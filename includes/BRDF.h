#ifndef BRDF_INCLUDED
#define BRDF_INCLUDED 1

function float Square(float x)
{
    return x * x;
}

// Calculates the Fresnel factor using Schlick's approximation
function vector Fresnel(vector specAlbedo; vector h; vector l)
{
    vector fresnel = specAlbedo + ({1.0, 1.0, 1.0} - specAlbedo) * pow((1.0 - clamp(dot(l, h), 0.0, 1.0)), 5.0);

    // Fade out spec entirely when lower than 0.1% albedo
    fresnel *= clamp(dot(specAlbedo, {333.0, 333.0, 333.0}), 0.0, 1.0);

    return fresnel;
}

// Calculates the Fresnel factor using Schlick's approximation
function vector Fresnel(vector specAlbedo; vector fresnelAlbedo; vector h; vector l)
{
    vector fresnel = specAlbedo + (fresnelAlbedo - specAlbedo) * pow((1.0f - clamp(dot(l, h), 0.0, 1.0)), 5.0);

    // Fade out spec entirely when lower than 0.1% albedo
    fresnel *= clamp(dot(specAlbedo, {333.0, 333.0, 333.0}), 0.0, 1.0);

    return fresnel;
}

// Helper for computing the GGX visibility term
function float GGX_V1(float m2; float nDotX)
{
    return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
}

// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. Based on "Microfacet Models for Refraction Through
// Rough Surfaces" [Walter 07]. m is roughness, n is the surface normal, h is the half vector,
// l is the direction to the light source, and specAlbedo is the RGB specular albedo
function float GGX_Specular(float m; vector n; vector h; vector v; vector l)
{
    float nDotH = clamp(dot(n, h), 0.0, 1.0);
    float nDotL = clamp(dot(n, l), 0.0, 1.0);
    float nDotV = clamp(dot(n, v), 0.0, 1.0);

    float nDotH2 = nDotH * nDotH;
    float m2 = m * m;

    // Calculate the distribution term
    float d = m2 / (PI * Square(nDotH * nDotH * (m2 - 1) + 1));

    // Calculate the matching visibility term
    float v1i = GGX_V1(m2, nDotL);
    float v1o = GGX_V1(m2, nDotV);
    float vis = v1i * v1o;

    return d * vis;
}

// Returns the PDF for a particular GGX sample after reflecting the view vector
// about a microfacet normal (includes the Jacobian for going from half vector to lighting vector)
function float GGX_PDF(vector n; vector h; vector v; float roughness)
{
    float nDotH = clamp(dot(n, h), 0.0, 1.0);
    float hDotV = clamp(dot(h, v), 0.0, 1.0);
    float m2 = roughness * roughness;
    float d = m2 / (PI * Square(nDotH * nDotH * (m2 - 1) + 1));
    float pM = d * nDotH;
    return pM / (4 * hDotV);
}

//-------------------------------------------------------------------------------------------------
// Calculates the lighting result for an analytical light source
//-------------------------------------------------------------------------------------------------
function vector StandardShading(vector normal; vector lightDir; vector lightColor; vector viewDir;
					            vector diffuseColor; vector specularColor; float roughness)
{
    vector lighting = diffuseColor / PI;

    float nDotL = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    if (nDotL > 0.0f)
    {
        float nDotV = clamp(dot(normal, viewDir), 0.0f, 1.0f);
        vector halfDir = normalize(viewDir + lightDir);

        vector fresnel = Fresnel(specularColor, halfDir, viewDir);

        float specular = GGX_Specular(roughness, normal, halfDir, viewDir, lightDir);
        lighting += specular * fresnel;
    }

    return lighting * nDotL * lightColor;
}

#endif // BRDF_INCLUDED