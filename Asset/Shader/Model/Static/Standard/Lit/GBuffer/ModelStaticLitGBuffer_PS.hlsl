#include "../../../../ModelCommon.hlsli"

SamplerState g_sampler : register(s0);

struct PSGBufferOutput
{
    float4 albedo        : SV_Target0;
    float4 normal        : SV_Target1;
    float4 worldPosition : SV_Target2;
};

float3 DecodeBC5NormalMap(float2 a_normalMapXY)
{
    // BC5のRGは0.0F~1.0Fで保存されているため、
    // 法線として使える-1.0F~+1.0Fの範囲へ戻す
    float2 l_normalXY = (a_normalMapXY * k_normalMapScale) - k_normalMapBias;
    
    // 使用しているNormalMapのGチャンネル向きに合わせる
    l_normalXY.y *= k_normalMapGreenChannelFlip;
    
    // BC5ではZ成分を保存していないためXYからZを復元する。
    const float l_normalZ = sqrt(saturate(1.0F - dot(l_normalXY, l_normalXY)));

    return normalize(float3(l_normalXY.x, l_normalXY.y, l_normalZ));
}

float3 ConvertNormalMapToWorldNormal(float3 a_normalMap, float3 a_worldNormal, float4 a_wordlTangent)
{
    const float3 l_tangentNormal = DecodeBC5NormalMap(a_normalMap.rg);
    
    const float3 l_normal = normalize(a_worldNormal);

    float3 l_tangent = l_normal = normalize(a_wordlTangent.xyz);
    
    // TangentがNormal方向に少し傾いている場合に備えて、
    // Normalと直行するように補正する
    l_tangent = normalize(l_tangent - (l_normal * dot(l_tangent, l_normal)));
    
    const float3 l_bitangent = normalize(cross(l_normal, l_tangent) * a_wordlTangent.w);
    
    const float3x3 l_tangentToWorldMatrix = float3x3(l_tangent, l_bitangent, l_normal);

    return normalize(mul(l_tangentNormal, l_tangentToWorldMatrix));
}

PSGBufferOutput main(MeshOutput a_input)
{
    PSGBufferOutput l_output = (PSGBufferOutput)0;

    Texture2D<float4> l_baseColorTexture = ResourceDescriptorHeap[g_baseColorTextureSRVIndex];
    Texture2D<float4> l_normalTexture    = ResourceDescriptorHeap[g_normalTextureSRVIndex];
    
    const float4 l_baseColor = l_baseColorTexture.Sample(g_sampler, a_input.uv);
    const float4 l_normalMap = l_normalTexture.Sample   (g_sampler, a_input.uv);

    const float3 l_wordlNoraml = ConvertNormalMapToWorldNormal(l_normalMap, a_input.worldNormal, a_input.worldTangent);
    
    // GBuffer0;
    // ベースカラー。DeferrredLightingPassでライト計算に使う。
    l_output.albedo = l_baseColor;

    // GBuffer1;
    // 法線。R8G8B8A8_UNORMにいれるため、-1 ~ +1を0 ~ 1にエンコードする
    l_output.normal = float4((normalize(l_wordlNoraml) * 0.5F) + 0.5F, 1.0F);

    // GBuffer2
    // ワールド座標。R16G16B16A16_FLOATなので、そのまま入れられる
    l_output.worldPosition = float4(a_input.worldPosition, 1.0F);

    return l_output;
}