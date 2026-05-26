#include "FinalPresent.hlsli"

float4 main(MeshOutput a_input) : SV_Target0
{
    Texture2D<float4> l_sceneColorTexture = ResourceDescriptorHeap[g_sceneColorTextureSRVIndex];

    return l_sceneColorTexture.Sample(g_finalPresentSampler, a_input.uv);
}