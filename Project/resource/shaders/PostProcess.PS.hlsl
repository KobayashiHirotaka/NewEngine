#include "PostProcess.hlsli"

Texture2D<float32_t> gLinearDepthTexture : register(t0);
Texture2D<float32_t4> gTexture : register(t1);
Texture2D<float32_t4> gHighIntensityTexture : register(t2);
Texture2D<float32_t4> gBlurTexture : register(t3);
Texture2D<float32_t4> gHighIntensityBlurTexture : register(t4);
Texture2D<float32_t4> gShrinkBlurTexture : register(t5);
Texture2D<float32_t4> gHighIntensityShrinkBlurTexture : register(t6);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

ConstantBuffer<Bloom> gBloomParameter : register(b0);
ConstantBuffer<Vignette> gVignetteParameter : register(b1);
ConstantBuffer<GrayScale> gGrayScaleParameter : register(b2);
ConstantBuffer<BoxFilter> gBoxFilterParameter : register(b3);
ConstantBuffer<GaussianFilter> gGaussianFilterParameter : register(b4);
ConstantBuffer<LuminanceBasedOutline> gLuminanceBasedOutlineParameter : register(b5);
ConstantBuffer<DepthBasedOutline> gDepthBasedOutlineParameter : register(b6);

//BoxFilter,GuassianFilter
static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const float32_t kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
};

//LuminanceOutline,DepthOutline
static const float32_t kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float32_t kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

static const float32_t PI = 3.14159265f;

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

float32_t Luminance(float32_t3 v)
{
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t depthColor = gLinearDepthTexture.Sample(gSampler, input.texcoord);
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t4 highIntensityColor = gHighIntensityTexture.Sample(gSampler, input.texcoord);
    float32_t4 blurColor = gBlurTexture.Sample(gSampler, input.texcoord);
    float32_t4 highIntensityBlurColor = gHighIntensityBlurTexture.Sample(gSampler, input.texcoord);
    float32_t4 shrinkBlurColor = gShrinkBlurTexture.Sample(gSampler, input.texcoord);
    float32_t4 highIntensityShrinkBlurColor = gHighIntensityShrinkBlurTexture.Sample(gSampler, input.texcoord);
    
    //BoxFilter
    if (gBoxFilterParameter.enable)
    {
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                
                textureColor.rgb += fetchColor * kKernel3x3[x][y];
            }

        }
    }
    
    //GaussianFilter
    if (gGaussianFilterParameter.enable)
    {
        float32_t weight = 0.0f;
        float32_t kernel3x3[3][3];
        
        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 1.0f);
                weight += kernel3x3[x][y];
            }
        }
        
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
        for (int32_t i = 0; i < 3; ++i)
        {
            for (int32_t j = 0; j < 3; ++j)
            {
                float32_t2 texcoord = input.texcoord + kIndex3x3[i][j] * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                
                textureColor.rgb += fetchColor * kernel3x3[i][j];
            }

        }
        
        textureColor.rgb *= rcp(weight);
    }
    
    
	//Bloom
    if (gBloomParameter.enable == true)
    {
        float4 color = textureColor;
        textureColor = (color + highIntensityColor + highIntensityBlurColor + highIntensityShrinkBlurColor) * gBloomParameter.intensity;
    }
    
    //Vignette
    if (gVignetteParameter.enable)
    {
        float4 color = textureColor;
        color = gTexture.Sample(gSampler, input.texcoord);
        
        float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        
        float vignette = correct.x * correct.y * 16.0f;
        
        vignette = saturate(pow(vignette, gVignetteParameter.intensity));
        
        textureColor.rgb *= vignette;
    }
    
    //GrayScale
    if (gGrayScaleParameter.enable)
    {
        float4 color = textureColor;
        float32_t value = dot(textureColor.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        textureColor.rgb = value * float32_t3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);
    }
    
     //LuminanceBasedOutline
    if (gLuminanceBasedOutlineParameter.enable)
    {
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        
        float32_t2 difference = float32_t2(0.0f, 0.0f);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                float32_t luminance = Luminance(fetchColor);
                
                difference.x += luminance * kPrewittHorizontalKernel[x][y];
                difference.y += luminance * kPrewittVerticalKernel[x][y];
            }
        }
        
        float32_t weight = length(difference);
        weight = saturate(weight * 6.0f);
        
        if (weight >= 0.5f)
        {
            textureColor.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
            textureColor.a = 1.0f;
        }
    }
    
     //DepthBasedOutline
    if (gDepthBasedOutlineParameter.enable)
    {
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        
        float32_t2 difference = float32_t2(0.0f, 0.0f);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                float32_t ndcDepth = gLinearDepthTexture.Sample(gSamplerPoint, texcoord);
                float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gDepthBasedOutlineParameter.projectionInverse);
                float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
                
                difference.x += viewZ * kPrewittHorizontalKernel[x][y];
                difference.y += viewZ * kPrewittVerticalKernel[x][y];
            }
        }
        
        float32_t weight = length(difference);
        weight = saturate(weight);
        
        if (weight >= 0.5f)
        {
            textureColor.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
            textureColor.a = 1.0f;
        }
    }

    output.color = textureColor;
    
    return output;
}