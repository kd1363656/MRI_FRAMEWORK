struct MeshOutput
{
    float4 position      : SV_Position;
    float3 worldPosition : POSITION0;
    float3 normal        : NORMAL0;
    float2 uv            : TEXCOORD0;
};

struct ModelMeshlet
{
    uint vertexOffset;
    uint triangleOffset;
    uint vertexCount;
    uint triangleCount;
};

cbuffer CBCamera : register(b0)
{
    row_major float4x4 g_viewMatrix;
    row_major float4x4 g_projectionMatrix;
    row_major float4x4 g_viewProjectionMatrix;
}

cbuffer CBModelObject : register(b1)
{
    row_major float4x4 g_worldMatrix;
}

static const float4 k_modelUnLitColor = { 1.0F, 1.0F, 1.0F, 1.0F };

static const uint k_triangleVertexCount      = 3U;
static const uint k_maxMeshletVertexCount    = 64U;
static const uint k_maxMeshletPrimitiveCount = 126U;

static const uint k_meshShaderThreadCountX = 1U;
static const uint k_meshShaderThreadCountY = 1U;
static const uint k_meshShaderThreadCountZ = 1U;

static const uint k_modelPositionVectorElementW = 1U;

static const uint k_secondPrimitiveVertexOffset = 1U;
static const uint k_thirdPrimitiveVertexOffset  = 2U;