#ifndef SH_INCLUDED
#define SH_INCLUDED 1

#include "$HIP/includes/BRDF.h"

#define CosineA0 PI
#define CosineA1 (2.0f * PI) / 3.0f
#define CosineA2 PI / 4.0f

struct SH4
{
    float coefficients[] = {0, 0, 0, 0};

    void Add(SH4 other)
    {
        for (int i = 0; i < 4; ++i)
            coefficients[i] += other.coefficients[i];
    }

    // Convolution with cosine kernel
    void ConvolveWithCosineKernel()
    {
        coefficients[0] *= CosineA0;

        for (int i = 1; i < 4; ++i)
            if (i < 4)
                coefficients[i] *= CosineA1;
            else if (i < 9)
                coefficients[i] *= CosineA2;
    }
};

struct SH4Color
{
    vector coefficients[] = {0, 0, 0, 0};

    void Add(SH4Color other)
    {
        for (int i = 0; i < 4; ++i)
            coefficients[i] += other.coefficients[i];
    }

    // Convolution with cosine kernel
    void ConvolveWithCosineKernel()
    {
        coefficients[0] *= CosineA0;

        for (int i = 1; i < 4; ++i)
            if (i < 4)
                coefficients[i] *= CosineA1;
            else if (i < 9)
                coefficients[i] *= CosineA2;
    }
};

struct SH9
{
    float coefficients[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    void Add(SH9 other)
    {
        for (int i = 0; i < 9; ++i)
            coefficients[i] += other.coefficients[i];
    }

    // Convolution with cosine kernel
    void ConvolveWithCosineKernel()
    {
        coefficients[0] *= CosineA0;

        for (int i = 1; i < 9; ++i)
            if (i < 4)
                coefficients[i] *= CosineA1;
            else if (i < 9)
                coefficients[i] *= CosineA2;
    }
};

struct SH9Color
{
    vector coefficients[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    void Add(SH9Color other)
    {
        for (int i = 0; i < 9; ++i)
            coefficients[i] += other.coefficients[i];
    }

    // Convolution with cosine kernel
    void ConvolveWithCosineKernel()
    {
        coefficients[0] *= CosineA0;

        for (int i = 1; i < 9; ++i)
            if (i < 4)
                coefficients[i] *= CosineA1;
            else if (i < 9)
                coefficients[i] *= CosineA2;
    }
};

//-------------------------------------------------------------------------------------------------
// Converts from 3-band to 2-band SH
//-------------------------------------------------------------------------------------------------
function SH4 ConvertToSH4(SH9 sh9)
{
    SH4 sh4;
    for (int i = 0; i < 4; ++i)
        sh4.coefficients[i] = sh9.coefficients[i];
    return sh4;
}

function SH4Color ConvertToSH4(SH9Color sh9)
{
    SH4Color sh4;
    for (int i = 0; i < 4; ++i)
        sh4.coefficients[i] = sh9.coefficients[i];
    return sh4;
}

//-------------------------------------------------------------------------------------------------
// Converts from 2-band to 3-band SH
//-------------------------------------------------------------------------------------------------
function SH9 ConvertToSH9(SH4 sh4)
{
    SH9 sh9;
    for (int i = 0; i < 4; ++i)
        sh9.coefficients[i] = sh4.coefficients[i];
    return sh9;
}

function SH9Color ConvertToSH9(SH4Color sh4)
{
    SH9Color sh9;
    for (int i = 0; i < 4; ++i)
        sh9.coefficients[i] = sh4.coefficients[i];
    return sh9;
}

// ================================================================================================
// SH4
// ================================================================================================

//-------------------------------------------------------------------------------------------------
// Projects a direction onto SH and convolves with a given kernel
//-------------------------------------------------------------------------------------------------
function SH4 ProjectOntoSH4(vector dir; float intensity; float A0; float A1)
{
    SH4 sh;

    // Band 0
    sh.coefficients[0] = 0.282095f * A0 * intensity;

    // Band 1
    sh.coefficients[1] = -0.488603f * dir.y * A1 * intensity;
    sh.coefficients[2] = 0.488603f * dir.z * A1 * intensity;
    sh.coefficients[3] = -0.488603f * dir.x * A1 * intensity;

    return sh;
}

function SH4Color ProjectOntoSH4Color(vector dir; vector color; float A0; float A1)
{
    SH4Color sh;

    // Band 0
    sh.coefficients[0] = 0.282095f * A0 * color;

    // Band 1
    sh.coefficients[1] = -0.488603f * dir.y * A1 * color;
    sh.coefficients[2] = 0.488603f * dir.z * A1 * color;
    sh.coefficients[3] = -0.488603f * dir.x * A1 * color;

    return sh;
}

function SH4 ProjectOntoSH4(vector dir; float intensity)
{
    return ProjectOntoSH4(dir, intensity, 1.0f, 1.0f);
}

function SH4Color ProjectOntoSH4Color(vector dir; vector color)
{
    return ProjectOntoSH4Color(dir, color, 1.0f, 1.0f);
}

function SH4 ProjectOntoSH4(vector dir)
{
    return ProjectOntoSH4(dir, 1.0f, 1.0f, 1.0f);
}

function SH4Color ProjectOntoSH4Color(vector dir)
{
    return ProjectOntoSH4Color(dir, 1.0f, 1.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------
// Computes the dot project of two SH4 vectors
//-------------------------------------------------------------------------------------------------
function float SHDotProduct(SH4 a; SH4 b)
{
	float result = 0.0f;

	for (int i = 0; i < 4; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

function vector SHDotProduct(SH4 a; SH4Color b)
{
	vector result = 0.0f;

	for (int i = 0; i < 4; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

function vector SHDotProduct(SH4Color a; SH4 b)
{
	vector result = 0.0f;

	for (int i = 0; i < 4; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

function vector SHDotProduct(SH4Color a; SH4Color b)
{
	vector result = 0.0f;

	for (int i = 0; i < 4; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

//-------------------------------------------------------------------------------------------------
// Projects a direction onto SH4 and dots it with another SH4 vector
//-------------------------------------------------------------------------------------------------
function vector EvalSH4(vector dir; SH4 sh)
{
	SH4 dirSH = ProjectOntoSH4(dir);
	return SHDotProduct(dirSH, sh);
}

function vector EvalSH4(vector dir; SH4Color sh)
{
	SH4Color dirSH = ProjectOntoSH4Color(dir);
	return SHDotProduct(dirSH, sh);
}

//-------------------------------------------------------------------------------------------------
// Projects a direction onto SH4, convolves with a cosine kernel, and dots it with another
// SH4 vector
//-------------------------------------------------------------------------------------------------
function float EvalSH4Irradiance(vector dir; SH4 sh)
{
	SH4 dirSH = ProjectOntoSH4(dir, 1.0f, CosineA0, CosineA1);
	return SHDotProduct(dirSH, sh);
}

function vector EvalSH4Irradiance(vector dir; SH4Color sh)
{
	SH4Color dirSH = ProjectOntoSH4Color(dir, 1.0f, CosineA0, CosineA1);
	return SHDotProduct(dirSH, sh);
}

// ================================================================================================
// SH9
// ================================================================================================

//-------------------------------------------------------------------------------------------------
// Projects a direction onto SH and convolves with a given kernel
//-------------------------------------------------------------------------------------------------
function SH9 ProjectOntoSH9(vector dir; float intensity; float A0; float A1; float A2)
{
    SH9 sh;

    // Band 0
    sh.coefficients[0] = 0.282095f * A0 * intensity;

    // Band 1
    sh.coefficients[1] = -0.488603f * dir.y * A1 * intensity;
    sh.coefficients[2] = 0.488603f * dir.z * A1 * intensity;
    sh.coefficients[3] = -0.488603f * dir.x * A1 * intensity;

    // Band 2
    sh.coefficients[4] = 1.092548f * dir.x * dir.y * A2 * intensity;
    sh.coefficients[5] = -1.092548f * dir.y * dir.z * A2 * intensity;
    sh.coefficients[6] = 0.315392f * (3.0f * dir.z * dir.z - 1.0f) * A2 * intensity;
    sh.coefficients[7] = -1.092548f * dir.x * dir.z * A2 * intensity;
    sh.coefficients[8] = 0.546274f * (dir.x * dir.x - dir.y * dir.y) * A2 * intensity;

    return sh;
}

function SH9Color ProjectOntoSH9Color(vector dir; vector color; float A0; float A1; float A2)
{
    SH9Color sh;

    // Band 0
    sh.coefficients[0] = 0.282095f * A0 * color;

    // Band 1
    sh.coefficients[1] = -0.488603f * dir.y * A1 * color;
    sh.coefficients[2] = 0.488603f * dir.z * A1 * color;
    sh.coefficients[3] = -0.488603f * dir.x * A1 * color;

    // Band 2
    sh.coefficients[4] = 1.092548f * dir.x * dir.y * A2 * color;
    sh.coefficients[5] = -1.092548f * dir.y * dir.z * A2 * color;
    sh.coefficients[6] = 0.315392f * (3.0f * dir.z * dir.z - 1.0f) * A2 * color;
    sh.coefficients[7] = -1.092548f * dir.x * dir.z * A2 * color;
    sh.coefficients[8] = 0.546274f * (dir.x * dir.x - dir.y * dir.y) * A2 * color;

    return sh;
}

function SH9 ProjectOntoSH9(vector dir; float intensity)
{
    return ProjectOntoSH9(dir, intensity, 1.0f, 1.0f, 1.0f);
}

function SH9Color ProjectOntoSH9Color(vector dir; vector color)
{
    return ProjectOntoSH9Color(dir, color, 1.0f, 1.0f, 1.0f);
}

function SH9 ProjectOntoSH9(vector dir)
{
    return ProjectOntoSH9(dir, 1.0f, 1.0f, 1.0f, 1.0f);
}

function SH9Color ProjectOntoSH9Color(vector dir)
{
    return ProjectOntoSH9Color(dir, 1.0f, 1.0f, 1.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------
// Computes the dot project of two SH4 vectors
//-------------------------------------------------------------------------------------------------
function float SHDotProduct(SH9 a; SH9 b)
{
	float result = 0.0f;

	for (int i = 0; i < 9; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

function vector SHDotProduct(SH9 a; SH9Color b)
{
	vector result = 0.0f;

	for (int i = 0; i < 9; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

function vector SHDotProduct(SH9Color a; SH9 b)
{
	vector result = 0.0f;

	for (int i = 0; i < 9; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

function vector SHDotProduct(SH9Color a; SH9Color b)
{
	vector result = 0.0f;

	for (int i = 0; i < 4; ++i)
		result += a.coefficients[i] * b.coefficients[i];

	return result;
}

//-------------------------------------------------------------------------------------------------
// Projects a direction onto SH9 and dots it with another SH9 vector
//-------------------------------------------------------------------------------------------------
function vector EvalSH9(vector dir; SH9 sh)
{
	SH9 dirSH = ProjectOntoSH9(dir);
	return SHDotProduct(dirSH, sh);
}

function vector EvalSH9(vector dir; SH9Color sh)
{
	SH9Color dirSH = ProjectOntoSH9Color(dir);
	return SHDotProduct(dirSH, sh);
}

//-------------------------------------------------------------------------------------------------
// Projects a direction onto SH9, convolves with a cosine kernel, and dots it with another
// SH9 vector
//-------------------------------------------------------------------------------------------------
function float EvalSH9Irradiance(vector dir; SH9 sh)
{
	SH9 dirSH = ProjectOntoSH9(dir, 1.0f, CosineA0, CosineA1, CosineA2);
	return SHDotProduct(dirSH, sh);
}

function vector EvalSH9Irradiance(vector dir; SH9Color sh)
{
	SH9Color dirSH = ProjectOntoSH9Color(dir, 1.0f, CosineA0, CosineA1, CosineA2);
	return SHDotProduct(dirSH, sh);
}

//-------------------------------------------------------------------------------------------------
// Evaluates the irradiance from a set of SH4 coeffecients using the non-linear fit from
// the paper by Graham Hazel from Geomerics.
// https://grahamhazel.com/blog/2017/12/22/converting-sh-radiance-to-irradiance/
//-------------------------------------------------------------------------------------------------
function float EvalSH4IrradianceGeomerics(vector dir; SH4 sh)
{
    float R0 = sh.coefficients[0];

    vector R1 = 0.5f * set(-sh.coefficients[3], -sh.coefficients[1], sh.coefficients[2]);
    float lenR1 = length(R1);

    float q = 0.5f * (1.0f + dot(R1 / lenR1, dir));

    float p = 1.0f + 2.0f * lenR1 / R0;
    float a = (1.0f - lenR1 / R0) / (1.0f + lenR1 / R0);

    return R0 * (a + (1.0f - a) * (p + 1.0f) * pow(abs(q), p));
}

function vector EvalSH4IrradianceGeomerics(vector dir; SH4Color sh)
{
    SH4 shr = SH4( array( sh.coefficients[0].x, sh.coefficients[1].x, sh.coefficients[2].x, sh.coefficients[3].x ) );
    SH4 shg = SH4( array( sh.coefficients[0].y, sh.coefficients[1].y, sh.coefficients[2].y, sh.coefficients[3].y ) );
    SH4 shb = SH4( array( sh.coefficients[0].z, sh.coefficients[1].z, sh.coefficients[2].z, sh.coefficients[3].z ) );

    return set(EvalSH4IrradianceGeomerics(dir, shr),
               EvalSH4IrradianceGeomerics(dir, shg),
               EvalSH4IrradianceGeomerics(dir, shb));
}

// ------------------------------------------------------------------------------------------------
// Computes the specular reflection from radiance encoded as a set of SH coefficients by
// convolving the radiance with another set of SH coefficients representing the current
// specular BRDF slice
// ------------------------------------------------------------------------------------------------
function vector ConvolutionSHSpecular(vector view; vector normal; vector specularColor; float sqrtRoughness; SH9Color shRadiance)
{
    // Make a local coordinate frame in tangent space or world space, with the x-axis
    // aligned with the view direction and the z-axis aligned with the normal
    vector zBasis = normal;
    vector yBasis = normalize(cross(zBasis, view));
    vector xBasis = normalize(cross(yBasis, zBasis));
    matrix3 localFrame = matrix3(set(xBasis, yBasis, zBasis));
    float viewAngle = clamp(dot(normal, view), 0.0f, 1.0f);

    // Look up coefficients from the SH lookup texture to make the SH BRDF
    SH9Color shBrdf;

    for (int i = 0; i < 3; ++i)
    {
        vector4 t0 = SHSpecularLookupA.SampleLevel(LinearSampler, float3(viewAngle, sqrtRoughness, specularColor[i]), 0.0f);
        vector2 t1 = SHSpecularLookupB.SampleLevel(LinearSampler, float3(viewAngle, sqrtRoughness, specularColor[i]), 0.0f);
        shBrdf.coefficients[0][i] = t0.x;
        shBrdf.coefficients[2][i] = t0.y;
        shBrdf.coefficients[3][i] = t0.z;
        shBrdf.coefficients[6][i] = t0.w;
        shBrdf.coefficients[7][i] = t1.x;
        shBrdf.coefficients[8][i] = t1.y;
    }

    // Transform the SH BRDF to tangent space/world space
    shBrdf = RotateSH9(shBrdf, localFrame);

    // Convolve the BRDF slice with the environment radiance
    return SHDotProduct(shBrdf, shRadiance);
}

// ------------------------------------------------------------------------------------------------
// Computes approximated specular from radiance encoded as a set of SH coefficients by
// approximating a directional light in the "dominant" direction.
// From "Precomputed Global Illumination in Frostbite"
// https://www.ea.com/frostbite/news/precomputed-global-illumination-in-frostbite
// ------------------------------------------------------------------------------------------------
function vector FrostbiteSHSpecular(vector view; vector normal; vector specularColor; float sqrtRoughness; SH4Color shRadiance)
{
    vector avgL1 = set(-dot(shRadiance.coefficients[3] / shRadiance.coefficients[0], vector(0.333f)),
                       -dot(shRadiance.coefficients[1] / shRadiance.coefficients[0], vector(0.333f)),
                        dot(shRadiance.coefficients[2] / shRadiance.coefficients[0], vector(0.333f)));
    avgL1 *= 0.5f;
    float avgL1len = length(avgL1);
    vector specDir = avgL1 / avgL1len;

    vector specLightColor = EvalSH4(specDir, shRadiance) * PI;

    sqrtRoughness = clamp(sqrtRoughness * 1.0f / sqrt(avgL1len), 0.0f, 1.0f);
    float roughness = sqrtRoughness * sqrtRoughness;

    return StandardShading(normal, specDir, specLightColor, view, 0.0f, specularColor, roughness);
}

// ------------------------------------------------------------------------------------------------
// A very rough SH specular approximation that converts the SH-projected radiance into 3 point
// lights oriented about the vertex normal
// ------------------------------------------------------------------------------------------------
function vector PunctualSHSpecular(vector view; vector normal; matrix3 tangentFrame; vector specularColor; float sqrtRoughness; SH9Color shRadiance)
{
    vector lightDirs[] = array
    (
        set(-1.0f / sqrt(6.0f), -1.0f / sqrt(2.0f), 1.0f / sqrt(3.0f)),
        set(-1.0f / sqrt(6.0f), 1.0f / sqrt(2.0f), 1.0f / sqrt(3.0f)),
        set(sqrt(2.0f / 3.0f), 0.0f, 1.0f / sqrt(3.0f))
    );

    vector result = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        vector lightDir = vtransform(lightDirs[i], tangentFrame);
        vector lightColor = EvalSH9(lightDir, shRadiance) * PI;

        result += StandardShading(normal, lightDir, lightColor, view, 0.0f, specularColor, sqrtRoughness * sqrtRoughness);
    }

    return result / 3.0f;
}

// ------------------------------------------------------------------------------------------------
// Computes approximated specular from radiance encoded as a set of SH coefficients by
// treating the SH radiance as a pre-filtered environment map
// ------------------------------------------------------------------------------------------------
function vector PrefilteredSHSpecular(vector view; vector normal; matrix3 tangentFrame; vector specularColor; float sqrtRoughness; SH9Color shRadiance)
{
    vector reflectDir = reflect(-view, normal);

    float roughness = sqrtRoughness * sqrtRoughness;

    // Pre-filter the SH radiance with the GGX NDF using a fitted approximation
    float l1Scale = 1.66711256633276f / (1.65715038133932f + roughness);
    float l2Scale = 1.56127990596116f / (0.96989757593282f + roughness) - 0.599972342361123f;

    SH9Color filteredSHRadiance = shRadiance;
    filteredSHRadiance.coefficients[1] *= l1Scale;
    filteredSHRadiance.coefficients[2] *= l1Scale;
    filteredSHRadiance.coefficients[3] *= l1Scale;
    filteredSHRadiance.coefficients[4] *= l2Scale;
    filteredSHRadiance.coefficients[5] *= l2Scale;
    filteredSHRadiance.coefficients[6] *= l2Scale;
    filteredSHRadiance.coefficients[7] *= l2Scale;
    filteredSHRadiance.coefficients[8] *= l2Scale;

    vector lookupDir = normalize(lerp(reflectDir, normal, clamp(roughness - 0.25f, 0.0f, 1.0f)));

    vector specLightColor = max(EvalSH9(lookupDir, filteredSHRadiance), 0.0f);

    float nDotV = clamp(dot(normal, view), 0.0f, 1.0f);
    vector2 AB = EnvSpecularLookup.SampleLevel(LinearSampler, float2(nDotV, sqrtRoughness), 0.0f);
    vector envBRDF = specularColor * AB.x + AB.y;

    return envBRDF * specLightColor;
}

#endif // SH_INCLUDED