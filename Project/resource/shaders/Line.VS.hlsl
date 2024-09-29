#include "Line.hlsli"

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

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float32_t4 position = input.position + float32_t4(gWorldTransform.world[3][0], gWorldTransform.world[3][1], gWorldTransform.world[3][2], 0.0f);
    output.position = mul(position, mul(gCamera.view, gCamera.projection));
   
    return output;
}