struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
    float32_t3 toEye : POSITION1;
    float32_t4 depth : COLOR0;
    float32_t3 cameraToPosition : POSITION2;
};