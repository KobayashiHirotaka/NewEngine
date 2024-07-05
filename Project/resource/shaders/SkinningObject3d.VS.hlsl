#include "Object3d.hlsli"

struct WorldTransform
{
    float32_t4x4 world;
    float32_t4x4 worldInverseTranspose;
};

struct Camera
{
    float32_t3 worldPosition;
    float32_t4x4 view;
    float32_t4x4 projection;
};

struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Skinned
{
    float32_t4 position;
    float32_t3 normal;
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
StructuredBuffer<Well> gMatrixPalette : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 weight : WEIGHT0;
    int32_t4 index : INDEX0;
};

Skinned Skinning(VertexShaderInput input)
{
    Skinned skinned;
  
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;
    
    skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    Skinned skinned = Skinning(input);
    
    if (input.weight.x == 0.0f && input.weight.y == 0.0f && input.weight.z == 0.0f && input.weight.w == 0.0f)
    {
        output.position = mul(input.position, mul(gWorldTransform.world, mul(gCamera.view, gCamera.projection)));
        output.texcoord = input.texcoord;
        output.normal = normalize(mul(input.normal, (float32_t3x3) gWorldTransform.worldInverseTranspose));
        output.worldPosition = mul(input.position, gWorldTransform.world).xyz;
        output.toEye = normalize(gCamera.worldPosition - mul(input.position, gWorldTransform.world).xyz);
    }
    else
    {
        output.position = mul(skinned.position, mul(gWorldTransform.world, mul(gCamera.view, gCamera.projection)));
        output.texcoord = input.texcoord;
        output.normal = normalize(mul(skinned.normal, (float32_t3x3) gWorldTransform.worldInverseTranspose));
        output.worldPosition = mul(skinned.position, gWorldTransform.world).xyz;
        output.toEye = normalize(gCamera.worldPosition - mul(input.position, gWorldTransform.world).xyz);
    }
    
    float z = (output.position.z - 0.1f) / (100.0f - 0.1f);
    output.depth = float32_t4(z, 0, 0, 0);

    return output;
}