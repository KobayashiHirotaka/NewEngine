struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct Bloom
{
    bool enable;
};

struct Vignette
{
    bool enable;
    float intensity;
};

struct GrayScale
{
    bool enable;
};

