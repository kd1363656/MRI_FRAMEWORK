#include "../../../ModelCommon.hlsli"

SamplerState g_baseColorSampler : register(s0);

float4 main(MeshOutput a_input) : SV_Target0
{
    Texture2D<float4> l_baseColorTexture = ResourceDescriptorHeap[g_baseColorTextureIndex];
    
    return l_baseColorTexture.Sample(g_baseColorSampler, a_input.uv);
}