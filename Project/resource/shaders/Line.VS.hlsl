#include "Line.hlsli"

struct Camera
{
    float32_t3 worldPosition;
    float32_t4x4 view;
    float32_t4x4 projection;
};

ConstantBuffer<Camera> gCamera : register(b0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gCamera.view, gCamera.projection));
   
    return output;
}