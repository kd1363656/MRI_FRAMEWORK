struct PSIN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 main(PSIN input) : SV_TARGET
{
    return float4(input.uv, 0.0f, 1.0f);
}