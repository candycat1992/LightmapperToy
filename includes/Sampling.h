function float Square(float x)
{
    return x * x;
}

// Calculates the Fresnel factor using Schlick's approximation
function vector Fresnel(vector specAlbedo, h, l)
{
    vector fresnel = specAlbedo + ({1.0, 1.0, 1.0} - specAlbedo) * pow((1.0 - clamp(dot(l, h), 0.0, 1.0)), 5.0);

    // Fade out spec entirely when lower than 0.1% albedo
    fresnel *= clamp(dot(specAlbedo, {333.0, 333.0, 333.0}), 0.0, 1.0);

    return fresnel;
}

// Calculates the Fresnel factor using Schlick's approximation
function vector Fresnel(vector specAlbedo, fresnelAlbedo, h, l)
{
    vector fresnel = specAlbedo + (fresnelAlbedo - specAlbedo) * pow((1.0f - clamp(dot(l, h), 0.0, 1.0)), 5.0);

    // Fade out spec entirely when lower than 0.1% albedo
    fresnel *= clamp(dot(specAlbedo, {333.0, 333.0, 333.0}), 0.0, 1.0);

    return fresnel;
}

// Helper for computing the GGX visibility term
function float GGX_V1(float m2, nDotX)
{
    return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
}

// Maps a value inside the square [0,1]x[0,1] to a value in a disk of radius 1 using concentric squares.
// This mapping preserves area, bi continuity, and minimizes deformation.
// Based off the algorithm "A Low Distortion Map Between Disk and Square" by Peter Shirley and
// Kenneth Chiu.
function vector2 SquareToConcentricDiskMapping(float x, y)
{
    float phi = 0.0f;
    float r = 0.0f;

    // -- (a,b) is now on [-1,1]?
    float a = 2.0f * x - 1.0f;
    float b = 2.0f * y - 1.0f;

    if(a > -b)                      // region 1 or 2
    {
        if(a > b)                   // region 1, also |a| > |b|
        {
            r = a;
            phi = (PI / 4.0f) * (b / a);
        }
        else                        // region 2, also |b| > |a|
        {
            r = b;
            phi = (PI / 4.0f) * (2.0f - (a / b));
        }
    }
    else                            // region 3 or 4
    {
        if(a < b)                   // region 3, also |a| >= |b|, a != 0
        {
            r = -a;
            phi = (PI / 4.0f) * (4.0f + (b / a));
        }
        else                        // region 4, |b| >= |a|, but a==0 and b==0 could occur.
        {
            r = -b;
            if(b != 0)
                phi = (PI / 4.0f) * (6.0f - (a / b));
            else
                phi = 0;
        }
    }

    vector2 result;
    result.x = r * cos(phi);
    result.y = r * sin(phi);
    return result;
}

// Returns a random cosine-weighted direction on the hemisphere around z = 1
function vector SampleCosineHemisphere(float u1, u2)
{
    vector2 uv = SquareToConcentricDiskMapping(u1, u2);
    float u = uv.x;
    float v = uv.y;

    // Project samples on the disk to the hemisphere to get a
    // cosine weighted distribution
    vector dir;
    float r = u * u + v * v;
    dir.x = u;
    dir.y = v;
    dir.z = sqrt(max(0.0f, 1.0f - r));

    return dir;
}

// Returns a microfacet normal (half direction) that can be be used to compute a
// reflected lighting direction. The PDF is equal to D(m) * dot(n, m)
function vector  SampleGGXMicrofacet(float roughness, u1, u2)
{
    float theta = atan2(roughness * sqrt(u1), sqrt(1 - u1));
    float phi = 2 * PI * u2;

    vector h;
    h.x = sin(theta) * cos(phi);
    h.y = sin(theta) * sin(phi);
    h.z = cos(theta);

    return h;
}

// Returns a world-space lighting direction for sampling a GGX distribution.
function vector SampleDirectionGGX(vector v, n; float roughness; matrix3 tangentToWorld; float u1, u2)
{
    vector h = SampleGGXMicrofacet(roughness, u1, u2);

    // Convert to world space
    h = normalize(vtransform(h, tangentToWorld));

    // Reflect the view vector about the microfacet normal
    float hDotV = dot(h, v);
    vector sampleDir = 2.0f * hDotV * h - v;
    return normalize(sampleDir);
}

// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. Based on "Microfacet Models for Refraction Through
// Rough Surfaces" [Walter 07]. m is roughness, n is the surface normal, h is the half vector,
// l is the direction to the light source, and specAlbedo is the RGB specular albedo
function float GGX_Specular(float m; vector n, h, v, l)
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
function float GGX_PDF(vector n, h, v; float roughness)
{
    float nDotH = clamp(dot(n, h), 0.0, 1.0);
    float hDotV = clamp(dot(h, v), 0.0, 1.0);
    float m2 = roughness * roughness;
    float d = m2 / (PI * Square(nDotH * nDotH * (m2 - 1) + 1));
    float pM = d * nDotH;
    return pM / (4 * hDotV);
}