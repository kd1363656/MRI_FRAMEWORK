cbuffer CBModelMaterial : register(b2)
{
    float4 g_baseColorFactor; 
    
    float  g_metallicFactor;
    float  g_roughnessFactor;
    float2 g_modelMaterialPadding;
    
    uint g_useBaseColorTexture;
    uint g_useNormalTextre;
    uint g_useRoughnessTexture;
    uint g_useMetallicTexture;
}

static const uint k_disableModelTexture = 0U;
static const uint k_enableModelTexture  = 1U;