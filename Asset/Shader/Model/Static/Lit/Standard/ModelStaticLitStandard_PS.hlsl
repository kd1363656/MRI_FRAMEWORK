#include "../../../ModelCommon.hlsli"
#include "../ModelStaticLitShaderCommon.hlsli"

Texture2D<float4> g_baseColorTexture : register(t4);
Texture2D<float4> g_normlaTexture    : register(t5);

SamplerState g_modelTextureSampler : register(s0);

float4 main(MeshOutput a_input) : SV_Target0
{
    // ベースカラーテクスチャをから色を取得
    const float4 l_baseColor = g_baseColorTexture.Sample(g_modelTextureSampler, a_input.uv);

    float3 l_worldNormal = normalize(a_input.normal);

    if (g_useNormalTexture != k_disableNormalTexture)
    {
        const float3 l_normalTextureColor = g_normlaTexture.Sample(g_modelTextureSampler, a_input.uv).xyz;
        
        l_worldNormal = ConvertNormalTextureToWorldNormal(l_worldNormal,
                                                           a_input.worldPosition,
                                                           l_normalTextureColor,
                                                           a_input.uv);
    }

    return l_baseColor;
}