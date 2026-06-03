struct MeshOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

cbuffer CBDeferredLightingPass : register(b0)
{
    uint g_gBufferAlbedoTextureSRVIndex;
    uint g_gBufferNormalTextureSRVIndex;
    uint g_gBufferWorldPositionTextureSRVIndex;
    uint g_padding;
}

SamplerState g_deferredLightingSampler : register(s0);

static const uint k_deferredLightingMeshShaderThreadCountX = 1U;
static const uint k_deferredLightingMeshShaderThreadCountY = 1U;
static const uint k_deferredLightingMeshShaderThreadCountZ = 1U;

static const uint k_deferredLightingVertexCount    = 3U;
static const uint k_deferredLightingPrimitiveCount = 1U;

static const uint k_deferredLightingPrimitiveIndex = 0U;

static const uint k_deferredLightingVertexIndexZero = 0U;
static const uint k_deferredLightingVertexIndexOne  = 1U;
static const uint k_deferredLightingVertexIndexTwo  = 2U;

static const float k_deferredLightingNDCLeft   = -1.0F;
static const float k_deferredLightingNDCRight  =  3.0F;
static const float k_deferredLightingNDCBottom = -1.0F;
static const float k_deferredLightingNDCTop    =  3.0F;

static const float k_deferredLightingUVLeft   =  0.0F;
static const float k_deferredLightingUVRight  =  2.0F;
static const float k_deferredLightingUVBottom =  1.0F;
static const float k_deferredLightingUVTop    = -1.0F;

static const float k_deferredLightingPositionZ = 0.0F;
static const float k_deferredLightingPositionW = 1.0F;