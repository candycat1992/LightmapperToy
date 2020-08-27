#ifndef UTILITIES_INCLUDED
#define UTILITIES_INCLUDED 1

function vector SampleEnvironment(vector rayDir)
{
    matrix envTrans = maketransform(0, 0, {0, 0, 0}, set(0, -chf(chs("../env_light_path") + "/ry"), 0));
    return environment(ch(chs("../env_light_path") + "/env_map"), rayDir * envTrans);
}

function vector SampleTexture2D(string path; float u; float v)
{
    return texture(path, u, v, "filter", "box", "wrap", "clamp");
}

function vector4 SampleTexture2D(string path; float u; float v)
{
    return texture(path, u, v, "filter", "box", "wrap", "clamp");
}

function vector4 SampleTexture3D(string path; float u; float v; float w; int size)
{
    float invSize = 1.0 / size;
    float depth = (1.0 - w) * (size - 1);
    int index = floor(depth);
    float fract = frac(depth);
    
    vector4 texel0 = SampleTexture2D(path, u, (index + v) * invSize);
    vector4 texel1 = SampleTexture2D(path, u, (index + 1 + v) * invSize);

    return lerp(texel0, texel1, fract);
}

function vector2 SampleTexture3D(string path; float u; float v; float w; int size)
{
    float invSize = 1.0 / size;
    float depth = (1.0 - w) * (size - 1);
    int index = floor(depth);
    float fract = frac(depth);
    
    vector4 texel0 = SampleTexture2D(path, u, (index + v) * invSize);
    vector4 texel1 = SampleTexture2D(path, u, (index + 1 + v) * invSize);
    
    return lerp(set(texel0.x, texel0.y), set(texel1.x, texel1.y), fract);
}

#endif // UTILITIES_INCLUDED