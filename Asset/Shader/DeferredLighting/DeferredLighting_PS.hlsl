#include "DeferredLighting.hlsli"

float4 main(MeshOutput a_input) : SV_Target0
{
    Texture2D<float4> l_gBufferAlbedoTexture = ResourceDescriptorHeap[g_gBufferAlbedoTextureSRVIndex];
    
    return l_gBufferAlbedoTexture.Sample(g_deferredLightingSampler, a_input.uv);
}