#include "DeferredLighting.hlsli"

static const float k_deferredLightingInvalidWorldPositionW = 0.0F;
static const float4 k_deferredLightingDebugBackgroundColor = float4(1.0F, 1.0F, 1.0F, 1.0F);

float4 main(MeshOutput a_input) : SV_Target0
{
    Texture2D<float4> l_gBufferAlbedoTexture        = ResourceDescriptorHeap[g_gBufferAlbedoTextureSRVIndex];
    Texture2D<float4> l_gBufferWorldPositionTexture = ResourceDescriptorHeap[g_gBufferWorldPositionTextureSRVIndex];
    
    const float4 l_gBufferWorldPosition = l_gBufferWorldPositionTexture.Sample(g_deferredLightingSampler, a_input.uv);
    
    if (l_gBufferWorldPosition.w <= k_deferredLightingInvalidWorldPositionW)
    {
        discard;
    }
    
    return l_gBufferAlbedoTexture.Sample(g_deferredLightingSampler, a_input.uv);
}