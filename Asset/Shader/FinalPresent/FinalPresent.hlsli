struct MeshOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

cbuffer CBFinalPresent : register(b0)
{
    uint g_sceneColorTextureSRVIndex;
    float3 g_padding;
};

static const uint k_finalPresentMeshShaderThreadCountX = 1U;
static const uint k_finalPresentMeshShaderThreadCountY = 1U;
static const uint k_finalPresentMeshShaderThreadCountZ = 1U;

static const uint k_finalPresentVertexCount    = 3U;
static const uint k_finalPresentPrimitiveCount = 1U;

static const uint k_finalPresentPrimitiveIndex = 0U;

static const uint k_finalPresentVertexIndexZero = 0U;
static const uint k_finalPresentVertexIndexOne  = 1U;
static const uint k_finalPresentVertexIndexTwo  = 2U;

static const float k_finalPresentNDCLeft   = -1.0F;
static const float k_finalPresentNDCRight  =  3.0F;
static const float k_finalPresentNDCBottom = -1.0F;
static const float k_finalPresentNDCTop    =  3.0F;

static const float k_finalPresentUVLeft   =  0.0F;
static const float k_finalPresentUVRight  =  2.0F;
static const float k_finalPresentUVBottom =  1.0F;
static const float k_finalPresentUVTop    = -1.0F;

static const float k_finalPresentPositionZ = 0.0F;
static const float k_finalPresentPositionW = 1.0F;

SamplerState g_finalPresentSampler : register(s0);