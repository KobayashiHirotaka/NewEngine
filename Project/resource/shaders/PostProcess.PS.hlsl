#include "PostProcess.hlsli"

Texture2D<float32_t> gLinearDepthTexture : register(t0);
Texture2D<float32_t4> gTexture : register(t1);
Texture2D<float32_t4> gHighIntensityTexture : register(t2);
Texture2D<float32_t4> gBlurTexture : register(t3);
Texture2D<float32_t4> gHighIntensityBlurTexture : register(t4);
Texture2D<float32_t4> gShrinkBlurTexture : register(t5);
Texture2D<float32_t4> gHighIntensityShrinkBlurTexture : register(t6);
SamplerState gSampler : register(s0);

ConstantBuffer<Bloom> gBloomParameter : register(b0);
ConstantBuffer<Vignette> gVignetteParameter : register(b1);
ConstantBuffer<GrayScale> gGrayScaleParameter : register(b2);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

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
   
	//Bloom
    if (gBloomParameter.enable == true)
    {
        float4 color = textureColor;
        textureColor = color + highIntensityColor + highIntensityBlurColor + highIntensityShrinkBlurColor;
    }
    
    //ビネット
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

    output.color = textureColor;
    
    return output;
}