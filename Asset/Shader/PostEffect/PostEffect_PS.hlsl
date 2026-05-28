#include "PostEffect.hlsli"

float4 main(MeshOutput a_input) : SV_Target0
{
    Texture2D<float4> l_sourceTexture = ResourceDescriptorHeap[g_sourceTextureSRVIndex];
    
    float4 l_color = l_sourceTexture.Sample(g_postEffectSampler, a_input.uv);
    
    l_color.rgb *= k_postEffectBrightness;

    return l_color;
    
    // グレースケール

    //Texture2D<float4> l_sourceTexture = ResourceDescriptorHeap[g_sourceTextureSRVIndex];
    
    //float4 l_color = l_sourceTexture.Sample(g_postEffectSampler, a_input.uv);
    
    //float l_gray = dot(l_color.rgb, float3(0.299, 0.587, 0.114));
    
    //return float4(l_gray, l_gray, l_gray, l_sourceTexture.Sample(g_postEffectSampler, a_input.uv).a);
}