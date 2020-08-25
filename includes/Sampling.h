#ifndef SAMPLING_INCLUDED
#define SAMPLING_INCLUDED 1

// Maps a value inside the square [0,1]x[0,1] to a value in a disk of radius 1 using concentric squares.
// This mapping preserves area, bi continuity, and minimizes deformation.
// Based off the algorithm "A Low Distortion Map Between Disk and Square" by Peter Shirley and
// Kenneth Chiu.
function vector2 SquareToConcentricDiskMapping(float x; float y)
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

// Returns a random direction on the hemisphere around z = 1
function vector SampleDirectionHemisphere(float u1; float u2)
{
    float z = u1;
    float r = sqrt(max(0.0f, 1.0f - z * z));
    float phi = 2 * PI * u2;
    float x = r * cos(phi);
    float y = r * sin(phi);

    return set(x, y, z);
}

// Returns a random cosine-weighted direction on the hemisphere around z = 1
function vector SampleCosineHemisphere(float u1; float u2)
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
function vector  SampleGGXMicrofacet(float roughness; float u1; float u2)
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
function vector SampleDirectionGGX(vector v; vector n; float roughness; matrix3 tangentToWorld; float u1; float u2)
{
    vector h = SampleGGXMicrofacet(roughness, u1, u2);

    // Convert to world space
    h = normalize(vtransform(h, tangentToWorld));

    // Reflect the view vector about the microfacet normal
    float hDotV = dot(h, v);
    vector sampleDir = 2.0f * hDotV * h - v;
    return normalize(sampleDir);
}

#endif // SAMPLING_INCLUDED