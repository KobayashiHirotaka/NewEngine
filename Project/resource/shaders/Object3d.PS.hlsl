#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

struct DirectionLight
{
    int32_t enableLighting;
    int32_t lightingType;
    int32_t modelType;
    float32_t4 color; 
    float32_t3 direction; 
    float intensity; 
};

struct PointLight
{
    int32_t enableLighting;
    int32_t lightingType;
    int32_t modelType;
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    float radius;
    float decay;
};

struct SpotLight
{
    int32_t enableLighting;
    int32_t lightingType;
    int32_t modelType;
    float32_t4 color;
    float32_t3 direction;
    float32_t distance;
    float32_t3 position;
    float32_t intensity;
    float32_t decay;
    float32_t cosAngle;
    float32_t cosFalloffStart;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionLight> gDirectionalLight : register(b1);
ConstantBuffer<PointLight> gPointLight : register(b2);
ConstantBuffer<SpotLight> gSpotLight : register(b3);

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
    float32_t3 lightingColor = { 0.0f, 0.0f, 0.0f };
    output.depth = input.depth.x;
    
    if (textureColor.a == 0)
    {
        discard;
    }
   
    //DirectionalLight
    if (gDirectionalLight.enableLighting != 0)
    {
        float cos = 0.0f;
        if (gDirectionalLight.lightingType == 0)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        else if (gDirectionalLight.lightingType == 1)
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        
        float32_t specularPow = 0.0f;
        if (gDirectionalLight.modelType == 0)
        {
            float32_t3 toEye = input.toEye;
            float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        
            float32_t RdotE = dot(reflectLight, toEye);
            specularPow = pow(saturate(RdotE), gMaterial.shininess);
        }
        else if (gDirectionalLight.modelType == 1)
        {
            float32_t3 toEye = input.toEye;
            float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
            
            float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NdotH), gMaterial.shininess);
        }
        
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        lightingColor += diffuse + specular;
        
    }
    
    //PointLight
    if (gPointLight.enableLighting != 0)
    {
        float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.direction);
        
        float32_t distance = length(gPointLight.direction - input.worldPosition);
        
        float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
        
        float cos = 0.0f;
        if (gPointLight.lightingType == 0)
        {
            cos = saturate(dot(normalize(input.normal), -pointLightDirection));
        }
        else if (gPointLight.lightingType == 1)
        {
            float NdotL = dot(normalize(input.normal), -pointLightDirection);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        
        float32_t specularPow = 0.0f;
        if (gPointLight.modelType == 0)
        {
            float32_t3 toEye = input.toEye;
            float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
        
            float32_t RdotE = dot(reflectLight, toEye);
            specularPow = pow(saturate(RdotE), gMaterial.shininess);
        }
        else if (gPointLight.modelType == 1)
        {
            float32_t3 toEye = input.toEye;
            float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
            
            float32_t3 halfVector = normalize(-pointLightDirection + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NdotH), gMaterial.shininess);
        }
        
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
            
        float32_t3 specular = gPointLight.color.rgb * gPointLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
        
        lightingColor += diffuse + specular;
        
    }
    
    //SpotLight
    if (gSpotLight.enableLighting != 0)
    {
        float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
        
        float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
        
        float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
        
        float cos = 0.0f;
        if (gSpotLight.lightingType == 0)
        {
            cos = saturate(dot(normalize(input.normal), -spotLightDirectionOnSurface));
        }
        else if (gSpotLight.lightingType == 1)
        {
            float NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        
        float32_t specularPow = 0.0f;
        if (gSpotLight.modelType == 0)
        {
            float32_t3 toEye = input.toEye;
            float32_t3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
        
            float32_t RdotE = dot(reflectLight, toEye);
            specularPow = pow(saturate(RdotE), gMaterial.shininess);
        }
        else if (gSpotLight.modelType == 1)
        {
            float32_t3 toEye = input.toEye;
            float32_t3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
            
            float32_t3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NdotH), gMaterial.shininess);
        }
        
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity * falloffFactor;
            
        float32_t3 specular = gSpotLight.color.rgb * gSpotLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f) * falloffFactor;
        
        lightingColor += diffuse + specular;
        
    }
     
    output.color.rgb = lightingColor;
    output.color.a = gMaterial.color.a * textureColor.a;
   
    if (textureColor.a == 0)
    {
        discard;
    }
    
    return output;
}