#include "MultiPass.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0; 
    float4 highIntensity : SV_TARGET1;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = textureColor;
    
    float luminance = dot(output.color.rgb, float3(0.299, 0.587, 0.114));
    
    if (luminance > 0.8)
    {
        output.highIntensity = luminance;
    }
    else
    {
        output.highIntensity = float4(0.0, 0.0, 0.0, 1.0);
    }
    
    return output;
}