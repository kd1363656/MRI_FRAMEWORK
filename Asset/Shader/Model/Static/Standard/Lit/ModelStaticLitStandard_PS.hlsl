#include "../../../ModelCommon.hlsli"

SamplerState g_sampler : register(s0);

// 仮ディレクショナルライト
// k_directionalLightDirectionは「ライトが進む方向」として扱う
static const float3 k_directionalLightDirection = normalize(float3(0.3F, -1.0F, 0.3F));
static const float3 k_directionalLightColor     = float3(1.0F, 1.0F, 1.0F);
static const float  k_directionalLightIntensity = 1.0F;
static const float3 k_ambientColor              = float3(1.0F, 1.0F, 1.0F);
static const float  k_ambientIntensity          = 0.25F;

float3 DecodeBC5NormalMap(float2 a_normalMapXY)
{
    // BC5のRGは0.0F~1.0Fで保存されているため、
    // 法線として使える-1.0F~+1.0Fの範囲へ戻す
    float2 l_normalXY = (a_normalMapXY * k_normalMapScale) - k_normalMapBias;

    // NormalMapのGチャンネル向き補正
    l_normalXY.y *= k_normalMapGreenChannelFlip;

    // BC5ではZ成分を保存していないため、XYからZを復元する
    const float l_normalZ = sqrt(saturate(1.0F - dot(l_normalXY, l_normalXY)));

    return normalize(float3(l_normalXY.x, l_normalXY.y, l_normalZ));
}

float3 ConvertNormalMapToWorldNormal(float3 a_normalMap, float3 a_worldNormal, float4 a_worldTangent)
{
    const float3 l_tangentNormal = DecodeBC5NormalMap(a_normalMap.rg);

    const float3 l_normal = normalize(a_worldNormal);

    float3 l_tangent = normalize(a_worldTangent.xyz);
    
    l_tangent = normalize(l_tangent - (l_normal * dot(l_tangent, l_normal)));

    const float3 l_bitangent = normalize(cross(l_normal, l_tangent) * a_worldTangent.w);

    const float3x3 l_tangentToWorldMatrix = float3x3(l_tangent, l_bitangent, l_normal );

    return normalize(mul(l_tangentNormal, l_tangentToWorldMatrix));
}

float4 main(MeshOutput a_input) : SV_Target0
{
    Texture2D<float4> l_baseColorTexture = ResourceDescriptorHeap[g_baseColorTextureIndex];
    Texture2D<float4> l_normalTexture    = ResourceDescriptorHeap[g_normalTextureIndex];

    const float4 l_baseColor = l_baseColorTexture.Sample(g_sampler, a_input.uv);
    const float3 l_normalMap = l_normalTexture.Sample   (g_sampler, a_input.uv).rgb;

    const float3 l_worldNormal = ConvertNormalMapToWorldNormal(l_normalMap, a_input.worldNormal, a_input.worldTangent);

    // dot(N, L)のLは「面からライトへ向かう方向」
    // k_directionalLightDirectionは「ライトが進む方向」なので符号を反転する
    const float3 l_lightDirection = normalize(-k_directionalLightDirection);

    const float l_nDotL = saturate(dot(l_worldNormal, l_lightDirection));

    const float3 l_diffuse = l_baseColor.rgb * k_directionalLightColor * l_nDotL * k_directionalLightIntensity;
    const float3 l_ambient = l_baseColor.rgb * k_ambientColor          * k_ambientIntensity;

    return float4(l_diffuse + l_ambient, l_baseColor.a);
}