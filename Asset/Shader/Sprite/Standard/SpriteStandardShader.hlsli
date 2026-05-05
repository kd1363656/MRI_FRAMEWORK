struct MeshOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

cbuffer SpritePassConstant : register(b0)
{
    row_major float4x4 g_projectionMatrix;
}

cbuffer CBSpriteDraw : register(b1)
{
    float4 g_color;
    
    
    float2 g_position;
    float2 g_scale;
    
    float2 g_pivot;
    float2 g_padding;
    
    uint4 g_sourceRECT;
}

Texture2D    g_baseColorTexture : register(t0);
SamplerState g_baseColorSampler : register(s0);