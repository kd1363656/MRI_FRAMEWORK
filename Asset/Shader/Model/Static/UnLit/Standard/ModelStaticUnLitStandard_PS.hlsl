#include "../../../ModelCommon.hlsli"

Texture2D<float4> g_baseColorTexture : register(t4);
SamplerState      g_baseColorSampler : register(s0);

float4 main(MeshOutput a_input) : SV_Target0
{
    return g_baseColorTexture.Sample(g_baseColorSampler, a_input.uv);
}