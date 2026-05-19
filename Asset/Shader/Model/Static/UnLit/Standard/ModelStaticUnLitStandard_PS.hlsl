#include "../../../ModelCommon.hlsli"
#include "../../../ModelMaterialCommon.hlsli"

Texture2D<float4> g_baseColorTexture : register(t4);
SamplerState      g_baseColorSampler : register(s0);

float4 main(MeshOutput a_input) : SV_Target0
{
    float4 l_baseColor = g_baseColorFactor;
    
    if (g_useBaseColorTexture == k_enableModelTexture)
    {
        l_baseColor *= g_baseColorTexture.Sample(g_baseColorSampler, a_input.uv);
    }

    return l_baseColor;
}