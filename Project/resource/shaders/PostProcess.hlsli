struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct Bloom
{
    bool enable;
    float intensity;
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

struct BoxFilter
{
    bool enable;
};

struct GaussianFilter
{
    bool enable;
};

struct LuminanceBasedOutline
{
    bool enable;
};

struct DepthBasedOutline
{
    bool enable;
    float32_t4x4 projectionInverse;
};

struct HSVFilter
{
    bool enable;
    float32_t hue;
    float32_t saturation;
    float32_t value;
};
