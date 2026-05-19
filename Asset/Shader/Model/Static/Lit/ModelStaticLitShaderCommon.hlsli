static const float k_normalTextureScale = 2.0F;
static const float k_normalTextureBias  = 1.0F;
static const float k_tbnEpsilon         = 0.00001F;

float3 ConvertNormalTextureToWorldNormal(float3 a_vertexWorldNormal,
                                         float3 a_worldPosition,
                                         float3 a_normalTextureColor,
                                         float2 a_uv,)
{
    const float3 l_vertexWorldNormal = normalize(a_vertexWorldNormal);
    
    const float3 l_positionDX = ddx(a_worldPosition);
    const float3 l_positionDY = ddy(a_worldPosition);

    const float2 l_uvDX = ddx(a_uv);
    const float2 l_uvDY = ddy(a_uv);

    const float l_determinant = l_uvDX.x * l_uvDY.y - l_uvDX.y * l_uvDY.x;
    
    if (abs(l_determinant) < k_tbnEpsilon)
    {
        return l_vertexWorldNormal;
    }
    
    const float3 l_tangent   = normalize(( l_positionDX * l_uvDY.y - l_positionDY * l_uvDX.y) / l_determinant);
    const float3 l_bitangent = normalize((-l_positionDX * l_uvDY.x + l_positionDY * l_uvDX.x) / l_determinant);

    const float3x3 l_tangentWorldMatrix = float3x3(l_tangent, l_bitangent, l_vertexWorldNormal);
    
    const float3 l_tangentNormal = normalize(a_normalTextureColor * k_normalTextureScale - k_normalTextureBias);
    
    return normalize(mul(l_tangentNormal, l_tangentWorldMatrix));
}