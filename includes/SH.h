#ifndef SH_INCLUDED
#define SH_INCLUDED 1

#include "$HIP/includes/BRDF.h"
#include "$HIP/includes/Utilities.h"

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

//-------------------------------------------------------------------------------------------------
// Rotates 2-band SH coefficients
//-------------------------------------------------------------------------------------------------
function SH4 RotateSH4(SH4 sh; matrix3 rotation)
{
    float r00 = getcomp(rotation, 0, 0);
    float r10 = getcomp(rotation, 0, 1);
    float r20 = getcomp(rotation, 0, 2);

    float r01 = getcomp(rotation, 1, 0);
    float r11 = getcomp(rotation, 1, 1);
    float r21 = getcomp(rotation, 1, 2);

    float r02 = getcomp(rotation, 2, 0);
    float r12 = getcomp(rotation, 2, 1);
    float r22 = getcomp(rotation, 2, 2);

    SH4 result;

    // Constant
    result.coefficients[0] = sh.coefficients[0];

    // Linear
    result.coefficients[1] =  r11 * sh.coefficients[1] - r12 * sh.coefficients[2] + r10 * sh.coefficients[3];
    result.coefficients[2] = -r21 * sh.coefficients[1] + r22 * sh.coefficients[2] - r20 * sh.coefficients[3];
    result.coefficients[3] =  r01 * sh.coefficients[1] - r02 * sh.coefficients[2] + r00 * sh.coefficients[3];

    return result;
}

function SH4Color RotateSH4(SH4Color sh; matrix3 rotation)
{
    float r00 = getcomp(rotation, 0, 0);
    float r10 = getcomp(rotation, 0, 1);
    float r20 = getcomp(rotation, 0, 2);

    float r01 = getcomp(rotation, 1, 0);
    float r11 = getcomp(rotation, 1, 1);
    float r21 = getcomp(rotation, 1, 2);

    float r02 = getcomp(rotation, 2, 0);
    float r12 = getcomp(rotation, 2, 1);
    float r22 = getcomp(rotation, 2, 2);

    SH4Color result;

    // Constant
    result.coefficients[0] = sh.coefficients[0];

    // Linear
    result.coefficients[1] =  r11 * sh.coefficients[1] - r12 * sh.coefficients[2] + r10 * sh.coefficients[3];
    result.coefficients[2] = -r21 * sh.coefficients[1] + r22 * sh.coefficients[2] - r20 * sh.coefficients[3];
    result.coefficients[3] =  r01 * sh.coefficients[1] - r02 * sh.coefficients[2] + r00 * sh.coefficients[3];

    return result;
}

//-------------------------------------------------------------------------------------------------
// Computes the "optimal linear direction" for a set of SH coefficients
//-------------------------------------------------------------------------------------------------
function vector OptimalLinearDirection(SH4 sh)
{
    float x = sh.coefficients[3];
    float y = sh.coefficients[1];
    float z = sh.coefficients[2];
    return normalize(set(x, y, z));
}

function vector OptimalLinearDirection(SH4Color sh)
{
    float x = dot(-sh.coefficients[3], set(0.3f, 0.59f, 0.11f));
    float y = dot(-sh.coefficients[1], set(0.3f, 0.59f, 0.11f));
    float z = dot( sh.coefficients[2], set(0.3f, 0.59f, 0.11f));
    return normalize(set(x, y, z));
}

//-------------------------------------------------------------------------------------------------
// Computes the direction and color of a directional light and a ambient color that approximates a set of SH
// coefficients
//-------------------------------------------------------------------------------------------------
function void ApproximateAmbientAndDirectionalLight(SH4Color sh; vector ambient; vector direction; vector color)
{
    direction = OptimalLinearDirection(sh);
    SH4 dirSH = ProjectOntoSH4(direction, 1.0f);

#if 1 // Uses Sébastien Lagarde's method from https://seblagarde.wordpress.com/2011/10/09/dive-in-sh-buffer-idea/

    float denom = SHDotProduct(dirSH, dirSH);
    vector numer = SHDotProduct(dirSH, sh);
    color = max(numer / denom, 0.0);

    // Subtract dominant light from original lighting environment
    sh.coefficients[0] -= dirSH.coefficients[0] * color;

    // With the remaining light, fit an ambient light
    float shAmbient = 0.282095f;
    denom = shAmbient * shAmbient;

    // Find the color of the ambient light
    ambient = sh.coefficients[0] * shAmbient / denom;
    //ambient = sh.coefficients[0] * shAmbient * PI;

#else // Uses Peter Pike-Sloan's method from "Stupid SH Tricks"

    sh.coefficients[0] *= PI;
    sh.coefficients[1] *= 2.0 * PI / 3.0;
    sh.coefficients[2] *= 2.0 * PI / 3.0;
    sh.coefficients[3] *= 2.0 * PI / 3.0;

    float dirSH0 = dirSH.coefficients[0];
    vector sh0 = sh.coefficients[0];

    dirSH.coefficients[0] = 0.0f;
    sh.coefficients[0] = 0.0f;

    color = max(SHDotProduct(dirSH, sh) * 867.0f / (316.0f * PI), 0.0);
    ambient = (sh0 - color * (8.0f * sqrt(PI) / 17.0)) * (sqrt(PI) / 2.0);

#endif
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
// Rotates 3-band SH coefficients
//-------------------------------------------------------------------------------------------------
function SH9 RotateSH9(SH9 sh; matrix3 rotation)
{
    float r00 = getcomp(rotation, 0, 0);
    float r10 = getcomp(rotation, 0, 1);
    float r20 = getcomp(rotation, 0, 2);

    float r01 = getcomp(rotation, 1, 0);
    float r11 = getcomp(rotation, 1, 1);
    float r21 = getcomp(rotation, 1, 2);

    float r02 = getcomp(rotation, 2, 0);
    float r12 = getcomp(rotation, 2, 1);
    float r22 = getcomp(rotation, 2, 2);

    SH9 result;

    // Constant
    result.coefficients[0] = sh.coefficients[0];

    // Linear
    result.coefficients[1] =  r11 * sh.coefficients[1] - r12 * sh.coefficients[2] + r10 * sh.coefficients[3];
    result.coefficients[2] = -r21 * sh.coefficients[1] + r22 * sh.coefficients[2] - r20 * sh.coefficients[3];
    result.coefficients[3] =  r01 * sh.coefficients[1] - r02 * sh.coefficients[2] + r00 * sh.coefficients[3];

    // Quadratic
    float t41 = r01 * r00;
    float t43 = r11 * r10;
    float t48 = r11 * r12;
    float t50 = r01 * r02;
    float t55 = r02 * r02;
    float t57 = r22 * r22;
    float t58 = r12 * r12;
    float t61 = r00 * r02;
    float t63 = r10 * r12;
    float t68 = r10 * r10;
    float t70 = r01 * r01;
    float t72 = r11 * r11;
    float t74 = r00 * r00;
    float t76 = r21 * r21;
    float t78 = r20 * r20;

    float v173 = 0.1732050808e1f;
    float v577 = 0.5773502693e0f;
    float v115 = 0.1154700539e1f;
    float v288 = 0.2886751347e0f;
    float v866 = 0.8660254040e0f;

    float r[];
    resize(r, 25);
    r[0] = r11 * r00 + r01 * r10;
    r[1] = -r01 * r12 - r11 * r02;
    r[2] = v173 * r02 * r12;
    r[3] = -r10 * r02 - r00 * r12;
    r[4] = r00 * r10 - r01 * r11;
    r[5] = -r11 * r20 - r21 * r10;
    r[6] = r11 * r22 + r21 * r12;
    r[7] = -v173 * r22 * r12;
    r[8] = r20 * r12 + r10 * r22;
    r[9] = -r10 * r20 + r11 * r21;
    r[10] = -v577* (t41 + t43) + v115 * r21 * r20;
    r[11] = v577* (t48 + t50) - v115 * r21 * r22;
    r[12] = -0.5f * (t55 + t58) + t57;
    r[13] = v577 * (t61 + t63) - v115 * r20 * r22;
    r[14] = v288 * (t70 - t68 + t72 - t74) - v577 * (t76 - t78);
    r[15] = -r01 * r20 - r21 * r00;
    r[16] = r01 * r22 + r21 * r02;
    r[17] = -v173 * r22 * r02;
    r[18] = r00 * r22 + r20 * r02;
    r[19] = -r00 * r20 + r01 * r21;
    r[20] = t41 - t43;
    r[21] = -t50 + t48;
    r[22] = v866 * (t55 - t58);
    r[23] = t63 - t61;
    r[24] = 0.5f *(t74 - t68 - t70 + t72);

    for (int i = 0; i < 5; ++i)
    {
        int base = i * 5;
        result.coefficients[4 + i] = r[base + 0] * sh.coefficients[4] + r[base + 1] * sh.coefficients[5] +
                                     r[base + 2] * sh.coefficients[6] + r[base + 3] * sh.coefficients[7] +
                                     r[base + 4] * sh.coefficients[8];
    }

    return result;
}

//-------------------------------------------------------------------------------------------------
// Rotates 3-band SH coefficients
//-------------------------------------------------------------------------------------------------
function SH9Color RotateSH9(SH9Color sh; matrix3 rotation)
{
    float r00 = getcomp(rotation, 0, 0);
    float r10 = getcomp(rotation, 0, 1);
    float r20 = getcomp(rotation, 0, 2);

    float r01 = getcomp(rotation, 1, 0);
    float r11 = getcomp(rotation, 1, 1);
    float r21 = getcomp(rotation, 1, 2);

    float r02 = getcomp(rotation, 2, 0);
    float r12 = getcomp(rotation, 2, 1);
    float r22 = getcomp(rotation, 2, 2);

    SH9Color result;

    // Constant
    result.coefficients[0] = sh.coefficients[0];

    // Linear
    result.coefficients[1] =  r11 * sh.coefficients[1] - r12 * sh.coefficients[2] + r10 * sh.coefficients[3];
    result.coefficients[2] = -r21 * sh.coefficients[1] + r22 * sh.coefficients[2] - r20 * sh.coefficients[3];
    result.coefficients[3] =  r01 * sh.coefficients[1] - r02 * sh.coefficients[2] + r00 * sh.coefficients[3];

    // Quadratic
    float t41 = r01 * r00;
    float t43 = r11 * r10;
    float t48 = r11 * r12;
    float t50 = r01 * r02;
    float t55 = r02 * r02;
    float t57 = r22 * r22;
    float t58 = r12 * r12;
    float t61 = r00 * r02;
    float t63 = r10 * r12;
    float t68 = r10 * r10;
    float t70 = r01 * r01;
    float t72 = r11 * r11;
    float t74 = r00 * r00;
    float t76 = r21 * r21;
    float t78 = r20 * r20;

    float v173 = 0.1732050808e1f;
    float v577 = 0.5773502693e0f;
    float v115 = 0.1154700539e1f;
    float v288 = 0.2886751347e0f;
    float v866 = 0.8660254040e0f;

    float r[];
    resize(r, 25);
    r[0] = r11 * r00 + r01 * r10;
    r[1] = -r01 * r12 - r11 * r02;
    r[2] =  v173 * r02 * r12;
    r[3] = -r10 * r02 - r00 * r12;
    r[4] = r00 * r10 - r01 * r11;
    r[5] = - r11 * r20 - r21 * r10;
    r[6] = r11 * r22 + r21 * r12;
    r[7] = -v173 * r22 * r12;
    r[8] = r20 * r12 + r10 * r22;
    r[9] = -r10 * r20 + r11 * r21;
    r[10] = -v577 * (t41 + t43) + v115 * r21 * r20;
    r[11] = v577 * (t48 + t50) - v115 * r21 * r22;
    r[12] = -0.5000000000e0f * (t55 + t58) + t57;
    r[13] = v577 * (t61 + t63) - v115 * r20 * r22;
    r[14] =  v288 * (t70 - t68 + t72 - t74) - v577 * (t76 - t78);
    r[15] = -r01 * r20 -  r21 * r00;
    r[16] = r01 * r22 + r21 * r02;
    r[17] = -v173 * r22 * r02;
    r[18] = r00 * r22 + r20 * r02;
    r[19] = -r00 * r20 + r01 * r21;
    r[20] = t41 - t43;
    r[21] = -t50 + t48;
    r[22] =  v866 * (t55 - t58);
    r[23] = t63 - t61;
    r[24] = 0.5000000000e0f * (t74 - t68 - t70 +  t72);

    for (int i = 0; i < 5; ++i)
    {
        int base = i * 5;
        result.coefficients[4 + i] = r[base + 0] * sh.coefficients[4] + r[base + 1] * sh.coefficients[5] +
                                     r[base + 2] * sh.coefficients[6] + r[base + 3] * sh.coefficients[7] +
                                     r[base + 4] * sh.coefficients[8];
    }

    return result;
}

//-------------------------------------------------------------------------------------------------
// Evaluates the irradiance from a set of SH4 coefficients using the non-linear fit from
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

//-------------------------------------------------------------------------------------------------
// Evaluates the irradiance from a directional and ambient light extracted from SH coefficients
//-------------------------------------------------------------------------------------------------
function vector EvalSH4IrradianceAmbientAndHighlightDiretion(vector dir; SH4Color sh)
{
    vector ambient, direction, color;
    ApproximateAmbientAndDirectionalLight(sh, ambient, direction, color);

    return ambient + clamp(dot(dir, direction), 0.0f, 1.0f) * color;
}

function vector EvalSH4IrradianceAmbientAndHighlightDiretion(vector dir; SH9Color sh)
{
    SH4Color sh4 = ConvertToSH4(sh);
    return EvalSH4IrradianceAmbientAndHighlightDiretion(dir, sh4);
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
        vector4 t0 = SampleTexture3D(chs("../baker_sh_specular_a_path"), viewAngle, sqrtRoughness, specularColor[i], 32);
        vector2 t1 = SampleTexture3D(chs("../baker_sh_specular_b_path"), viewAngle, sqrtRoughness, specularColor[i], 32);
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
    vector AB = SampleTexture2D(chs("../baker_sh_specular_lut_path"), nDotV, sqrtRoughness);
    vector envBRDF = specularColor * AB.x + AB.y;

    return envBRDF * specLightColor;
}

//-------------------------------------------------------------------------------------------------
// Evaluates the irradiance from a directional and ambient light extracted from SH coefficients
//-------------------------------------------------------------------------------------------------
function vector AmbientAndHighlightDiretionSHSpecular(vector view; vector normal; vector specularColor; float sqrtRoughness; SH4Color shRadiance)
{
    float roughness = sqrtRoughness * sqrtRoughness;

    vector ambient, direction, color;
    ApproximateAmbientAndDirectionalLight(shRadiance, ambient, direction, color);

    return StandardShading(normal, direction, color, view, 0.0f, specularColor, roughness);
}

function vector AmbientAndHighlightDiretionSHSpecular(vector view; vector normal; vector specularColor; float sqrtRoughness; SH9Color shRadiance)
{
    SH4Color sh4Radiance = ConvertToSH4(shRadiance);
    return AmbientAndHighlightDiretionSHSpecular(view, normal, specularColor, sqrtRoughness, sh4Radiance);
}

#endif // SH_INCLUDED