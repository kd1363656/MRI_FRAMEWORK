#include "DeferredLighting.hlsli"

[outputtopology("triangle")]
[numthreads(k_deferredLightingMeshShaderThreadCountX, k_deferredLightingMeshShaderThreadCountY, k_deferredLightingMeshShaderThreadCountZ)]
void main(out vertices MeshOutput a_vertexList [k_deferredLightingVertexCount],
          out indices uint3 a_primitiveList[k_deferredLightingPrimitiveCount])
{
    // 画面残体を覆う巨大な三角形を一枚だけ出す
    // 四角形2毎よりも境界が出にくく、全画面描画に向いている
    SetMeshOutputCounts(k_deferredLightingVertexCount, k_deferredLightingPrimitiveCount);
    
     const float2 l_positionList[k_deferredLightingVertexCount] =
    {
        float2(k_deferredLightingNDCLeft,  k_deferredLightingNDCBottom),
        float2(k_deferredLightingNDCLeft,  k_deferredLightingNDCTop),
        float2(k_deferredLightingNDCRight, k_deferredLightingNDCBottom)
    };

    const float2 l_uvList[k_deferredLightingVertexCount] =
    {
        float2(k_deferredLightingUVLeft,  k_deferredLightingUVBottom),
        float2(k_deferredLightingUVLeft,  k_deferredLightingUVTop),
        float2(k_deferredLightingUVRight, k_deferredLightingUVBottom)
    };

    [unroll]
    for (uint l_vertexIndex = 0U; l_vertexIndex < k_deferredLightingVertexCount; ++l_vertexIndex)
    {
        a_vertexList[l_vertexIndex].position = float4(l_positionList[l_vertexIndex], k_deferredLightingPositionZ, k_deferredLightingPositionW);
        a_vertexList[l_vertexIndex].uv       = l_uvList[l_vertexIndex];
    }

    a_primitiveList[k_deferredLightingPrimitiveIndex] =
        uint3(k_deferredLightingVertexIndexZero, k_deferredLightingVertexIndexOne, k_deferredLightingVertexIndexTwo);
}