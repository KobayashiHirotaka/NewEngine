#include "VerticalBlur.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    float4 highIntensity : SV_TARGET1; 
};

struct Blur
{
    int32_t textureWidth;
    int32_t textureHeight;
    float4 blurWeights[2];
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gHighIntensityTexture : register(t1);
SamplerState gSampler : register(s0);

ConstantBuffer<Blur> gBlurParameter : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = float4(0, 0, 0, 0);
    output.highIntensity = float4(0, 0, 0, 0);
    float4 color = gTexture.Sample(gSampler, input.texcoord);
    float w, h, level;
   
    gTexture.GetDimensions(0, w, h, level);
    float dy = 1.0f / h;

    output.color += gBlurParameter.blurWeights[0] * color;
    output.color += gBlurParameter.blurWeights[0][1] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 1));
    output.color += gBlurParameter.blurWeights[0][1] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 1));
    output.color += gBlurParameter.blurWeights[0][2] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 2));
    output.color += gBlurParameter.blurWeights[0][2] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 2));
    output.color += gBlurParameter.blurWeights[0][3] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 3));
    output.color += gBlurParameter.blurWeights[0][3] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 3));
    output.color += gBlurParameter.blurWeights[1][0] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 4));
    output.color += gBlurParameter.blurWeights[1][0] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 4));
    output.color += gBlurParameter.blurWeights[1][1] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 5));
    output.color += gBlurParameter.blurWeights[1][1] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 5));
    output.color += gBlurParameter.blurWeights[1][2] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 6));
    output.color += gBlurParameter.blurWeights[1][2] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 6));
    output.color += gBlurParameter.blurWeights[1][3] * gTexture.Sample(gSampler, input.texcoord + float2(0, dy * 7));
    output.color += gBlurParameter.blurWeights[1][3] * gTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 7));
    output.color.a = color.a;
    
    output.highIntensity += gBlurParameter.blurWeights[0] * color;
    output.highIntensity += gBlurParameter.blurWeights[0][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 1));
    output.highIntensity += gBlurParameter.blurWeights[0][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 1));
    output.highIntensity += gBlurParameter.blurWeights[0][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 2));
    output.highIntensity += gBlurParameter.blurWeights[0][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 2));
    output.highIntensity += gBlurParameter.blurWeights[0][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 3));
    output.highIntensity += gBlurParameter.blurWeights[0][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 3));
    output.highIntensity += gBlurParameter.blurWeights[1][0] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 4));
    output.highIntensity += gBlurParameter.blurWeights[1][0] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 4));
    output.highIntensity += gBlurParameter.blurWeights[1][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 5));
    output.highIntensity += gBlurParameter.blurWeights[1][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 5));
    output.highIntensity += gBlurParameter.blurWeights[1][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 6));
    output.highIntensity += gBlurParameter.blurWeights[1][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 6));
    output.highIntensity += gBlurParameter.blurWeights[1][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, dy * 7));
    output.highIntensity += gBlurParameter.blurWeights[1][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(0, -dy * 7));
    output.highIntensity.a = color.a;
    
    return output;
}