#include "PostEffect.hlsli"

[outputtopology("triangle")]
[numthreads(k_postEffectMeshShaderThreadCountX, k_postEffectMeshShaderThreadCountY, k_postEffectMeshShaderThreadCountZ)]
void main(out vertices MeshOutput a_vertexList   [k_postEffectVertexCount],
          out indices uint3       a_primitiveList[k_postEffectPrimitiveCount])
{
    // フルスクリーン描画では、四角形を三角形2毎で作る方法もあるが、
    // 三角形2枚にすると対角線の境界付近でPixelShaderの実行単位が重複しやすい、
    // そのため、ここでは画面外まではみ出す巨大な三角形1毎で描画全体を覆う
    // 画面外にはみ出した部分はGPUのクリッピングで破棄される
    // これにより、三角形同士の境界がなくなり、PostEffect用の全画面描画に向いた形になる。
    
    // SetMeshOutputCounts(出力頂点数、
    //                     出力プリミティブ数);
    SetMeshOutputCounts(k_postEffectVertexCount, k_postEffectPrimitiveCount);
    
    const float2 l_positionList[k_postEffectVertexCount] =
    {
        float2(k_postEffectNDCLeft,  k_postEffectNDCBottom),
        float2(k_postEffectNDCLeft,  k_postEffectNDCTop),
        float2(k_postEffectNDCRight, k_postEffectNDCBottom)
    };

    const float2 l_uvList[k_postEffectVertexCount] =
    {
        float2(k_postEffectUVLeft,  k_postEffectUVBottom),
        float2(k_postEffectUVLeft,  k_postEffectUVTop),
        float2(k_postEffectUVRight, k_postEffectUVBottom)
    };

    [unroll]
    for (uint l_vertexIndex = 0U; l_vertexIndex < k_postEffectVertexCount; ++l_vertexIndex)
    {
        a_vertexList[l_vertexIndex].position = float4(l_positionList[l_vertexIndex], k_postEffectPositionZ, k_postEffectPositionW);
        a_vertexList[l_vertexIndex].uv       = l_uvList[l_vertexIndex];
    }

    a_primitiveList[k_postEffectPrimitiveIndex] = uint3(k_postEffectVertexIndexZero, k_postEffectVertexIndexOne, k_postEffectVertexIndexTwo);
}