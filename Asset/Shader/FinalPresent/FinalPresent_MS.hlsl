#include "FinalPresent.hlsli"

[outputtopology("triangle")]
[numthreads(k_finalPresentMeshShaderThreadCountX, k_finalPresentMeshShaderThreadCountY, k_finalPresentMeshShaderThreadCountZ)]
void main(out vertices MeshOutput a_vertexList [k_finalPresentVertexCount],
          out indices  uint3      a_primitiveLisst[k_finalPresentPrimitiveCount])
{
    // SetMeshOutputCounts(出力頂点数、
    //                     出力プリミティブ);
    SetMeshOutputCounts(k_finalPresentVertexCount, k_finalPresentPrimitiveCount);

    const float2 l_positionList[k_finalPresentVertexCount] = 
    {
        float2(-1.0F, -1.0F),
        float2(-1.0F,  3.0F),
        float2( 3.0F, -1.0F)
    };
    
    const float2 l_uvList[k_finalPresentVertexCount] = 
    {
        float2(0.0F,  1.0F),
        float2(0.0F, -1.0F),
        float2(2.0F,  1.0F)
    };

    [unroll]
    for (uint l_vertexIndex = 0U; l_vertexIndex < k_finalPresentVertexCount; ++l_vertexIndex)
    {    
        a_vertexList[l_vertexIndex].position = float4(l_positionList[l_vertexIndex], k_finalPresentPositionZ, k_finalPresentPositionW);
        a_vertexList[l_vertexIndex].uv       = l_uvList[l_vertexIndex];
    }

    a_primitiveLisst[0U] = uint3(k_finalPresentVertexIndexZero, k_finalPresentVertexIndexOne, k_finalPresentVertexIndexTwo);
}