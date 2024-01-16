#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
};

struct DirectionLight
{
    int32_t enableLighting;
    int32_t lightingMethod;
    float32_t4 color; //ライトの色
    float32_t3 direction; //ライトの向き
    float intensity; //輝度
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
    float depth : SV_TARGET1;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    output.color = gMaterial.color * textureColor;
    output.depth = input.depth.x;
    
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    if (textureColor.a == 0)
    {
        discard;
    }
    
    if (output.color.a == 0)
    {
        discard;
    }
    
    if (gDirectionalLight.enableLighting != 0)
    {
        if (gDirectionalLight.lightingMethod == 0)
        {
            float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            output.color.a = gMaterial.color.a * textureColor.a;
        }
        else if (gDirectionalLight.lightingMethod == 1)
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            output.color.a = gMaterial.color.a * textureColor.a;
        }
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}