struct MeshOutput
{
    float4 position      : SV_Position;
    float3 worldPosition : POSITION0;
    float3 worldNormal   : NORMAL0;
    float4 worldTangent  : TANGENT0;
    float2 uv            : TEXCOORD0;
};

struct ModelMeshlet
{
    uint vertexOffset;
    uint triangleOffset;
    uint vertexCount;
    uint triangleCount;
};

cbuffer CBCameraPass : register(b0)
{
    row_major matrix g_viewMatrix;
    row_major matrix g_projectionMatrix;
    row_major matrix g_viewProjectionMatrix;
}

cbuffer CBModelPerObject : register(b1)
{
    row_major matrix g_worldMatrix;
    
    uint g_baseColorTextureSRVIndex;
    uint g_normalTextureSRVIndex;
    uint g_vertexBufferSRVIndex;
    uint g_meshletBufferSRVIndex;
    
    uint   g_uniqueVertexIndexBufferSRVIndex;
    uint   g_primitiveIndexBufferSRVIndex;
    float2 g_padding;
}

cbuffer CBLightPass : register(b2)
{
    float3 g_directionalLightDirection;
    float  g_directionalLightIntensity;
    
    float3 g_directionLightColor;
    float  g_lightPadding;
    
    float3 g_ambientLightColor;
    float  g_ambientLightIntensity;
}

static const float4 k_modelUnLitColor = { 1.0F, 1.0F, 1.0F, 1.0F };

static const uint k_triangleVertexCount      = 3U;
static const uint k_maxMeshletVertexCount    = 64U;
static const uint k_maxMeshletPrimitiveCount = 126U;

static const uint k_meshShaderThreadCountX = 1U;
static const uint k_meshShaderThreadCountY = 1U;
static const uint k_meshShaderThreadCountZ = 1U;

static const uint k_modelPositionVectorElementW = 1U;
static const float k_directionVectorElementW    = 0.0F;

static const uint k_secondPrimitiveVertexOffset = 1U;
static const uint k_thirdPrimitiveVertexOffset  = 2U;

static const float k_normalMapScale            = 2.0F;
static const float k_normalMapBias             = 1.0F;
static const float k_normalMapGreenChannelFlip = 1.0F;