#include "HorizontalBlur.hlsli"

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
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);
    float w = gBlurParameter.textureWidth;
    float h = gBlurParameter.textureHeight;
    float dx = 1.0f / w;

    output.color += gBlurParameter.blurWeights[0] * baseColor;
    output.color += gBlurParameter.blurWeights[0][1] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 1, 0));
    output.color += gBlurParameter.blurWeights[0][1] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 1, 0));
    output.color += gBlurParameter.blurWeights[0][2] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 2, 0));
    output.color += gBlurParameter.blurWeights[0][2] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 2, 0));
    output.color += gBlurParameter.blurWeights[0][3] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 3, 0));
    output.color += gBlurParameter.blurWeights[0][3] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 3, 0));
    output.color += gBlurParameter.blurWeights[1][0] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 4, 0));
    output.color += gBlurParameter.blurWeights[1][0] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 4, 0));
    output.color += gBlurParameter.blurWeights[1][1] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 5, 0));
    output.color += gBlurParameter.blurWeights[1][1] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 5, 0));
    output.color += gBlurParameter.blurWeights[1][2] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 6, 0));
    output.color += gBlurParameter.blurWeights[1][2] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 6, 0));
    output.color += gBlurParameter.blurWeights[1][3] * gTexture.Sample(gSampler, input.texcoord + float2(dx * 7, 0));
    output.color += gBlurParameter.blurWeights[1][3] * gTexture.Sample(gSampler, input.texcoord + float2(-dx * 7, 0));
    output.color.a = baseColor.a;
    
    output.highIntensity += gBlurParameter.blurWeights[0] * baseColor;
    output.highIntensity += gBlurParameter.blurWeights[0][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 1, 0));
    output.highIntensity += gBlurParameter.blurWeights[0][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 1, 0));
    output.highIntensity += gBlurParameter.blurWeights[0][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 2, 0));
    output.highIntensity += gBlurParameter.blurWeights[0][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 2, 0));
    output.highIntensity += gBlurParameter.blurWeights[0][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 3, 0));
    output.highIntensity += gBlurParameter.blurWeights[0][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 3, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][0] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 4, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][0] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 4, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 5, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][1] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 5, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 6, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][2] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 6, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(dx * 7, 0));
    output.highIntensity += gBlurParameter.blurWeights[1][3] * gHighIntensityTexture.Sample(gSampler, input.texcoord + float2(-dx * 7, 0));
    output.highIntensity.a = baseColor.a;

    return output;
}