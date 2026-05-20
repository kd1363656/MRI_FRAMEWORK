cbuffer CBModelMaterial : register(b2)
{
    float4 g_baseColorFactor; 
    
    float2 g_modelMaterialPadding;
    float  g_metallicFactor;
    float  g_roughnessFactor;
    
    uint g_useBaseColorTexture;
    uint g_useNormalTexture;
    uint g_useRoughnessTexture;
    uint g_useMetallicTexture;
}

static const uint k_disableModelTexture = 0U;
static const uint k_enableModelTexture  = 1U;