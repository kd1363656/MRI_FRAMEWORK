struct MeshOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

cbuffer CBPostEffect : register(b0)
{
    uint   g_sourceTextureSRVIndex;
    float3 g_padding;
}

static const uint k_postEffectMeshShaderThreadCountX = 1U;
static const uint k_postEffectMeshShaderThreadCountY = 1U;
static const uint k_postEffectMeshShaderThreadCountZ = 1U;

static const uint k_postEffectVertexCount    = 3U;
static const uint k_postEffectPrimitiveCount = 1U;

static const uint k_postEffectPrimitiveIndex = 0U;

static const uint k_postEffectVertexIndexZero = 0U;
static const uint k_postEffectVertexIndexOne  = 1U;
static const uint k_postEffectVertexIndexTwo  = 2U;

static const float k_postEffectNDCLeft   = -1.0F;
static const float k_postEffectNDCRight  =  3.0F;
static const float k_postEffectNDCBottom = -1.0F;
static const float k_postEffectNDCTop    =  3.0F;

static const float k_postEffectUVLeft   =  0.0F;
static const float k_postEffectUVRight  =  2.0F;
static const float k_postEffectUVBottom =  1.0F;
static const float k_postEffectUVTop    = -1.0F;

static const float k_postEffectPositionZ = 0.0F;
static const float k_postEffectPositionW = 1.0F;

static const float k_postEffectBrightness = 1.0F;

SamplerState g_postEffectSampler : register(s0);