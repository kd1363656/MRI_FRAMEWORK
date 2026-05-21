#include "../../../ModelCommon.hlsli"
#include "../StandardModelCommon.hlsli"

[outputtopology("triangle")]
[numthreads(k_meshShaderThreadCountX, k_meshShaderThreadCountY, k_meshShaderThreadCountZ)]
void main(uint3                   a_groupID : SV_GroupID,
          out vertices MeshOutput a_vertexList[k_maxMeshletVertexCount],
          out indices  uint3      a_primitiveList[k_maxMeshletPrimitiveCount])
{
    // バインドレスでヒープから直接バッファを取得
    StructuredBuffer<StaticModelVertex> l_modelVertexBuffer       = ResourceDescriptorHeap[g_vertexBufferIndex];
    StructuredBuffer<ModelMeshlet>      l_modelMeshletBuffer      = ResourceDescriptorHeap[g_meshletBufferIndex];
    StructuredBuffer<uint>              l_uniqueVertexIndexBuffer = ResourceDescriptorHeap[g_uniqueVertexIndexBufferIndex];
    StructuredBuffer<uint>              l_primitiveIndexBuffer    = ResourceDescriptorHeap[g_primitiveIndexBufferIndex];
    
    const uint l_meshletIndex = a_groupID.x;
    
    const ModelMeshlet l_modelMeshlet = l_modelMeshletBuffer[l_meshletIndex];
    
    // 出力する頂点数と三角形の個数をセット
    SetMeshOutputCounts(l_modelMeshlet.vertexCount, l_modelMeshlet.triangleCount);

    for (uint l_vertexIndex = 0U; l_vertexIndex < l_modelMeshlet.vertexCount; ++l_vertexIndex)
    {
        const uint l_modelVertexIndex = l_uniqueVertexIndexBuffer[l_modelMeshlet.vertexOffset + l_vertexIndex];
        
        const StaticModelVertex l_modelVertex = l_modelVertexBuffer[l_modelVertexIndex];
        
        // ワールド座標とローカル座標を算出
        const float4 l_localPosition = float4(l_modelVertex.position, k_modelPositionVectorElementW);
        const float4 l_worldPosition = mul   (l_localPosition,        g_worldMatrix);

        // 法線は方向ベクトルなので、w = 0.0FとしてWorld変換する
        float3 l_worldNormal = normalize(mul(float4(l_modelVertex.normal, k_directionVectorElementW), g_worldMatrix).xyz);
        
        // 接線も方向ベクトルなので、w = 0.0FとしてWorld変換する
        float3 l_worldTangent = normalize(mul(float4(l_modelVertex.tangent.xyz,  k_directionVectorElementW), g_worldMatrix).xyz);
        
        // TangentがNormal方向に少し傾いている場合に備えて、Normalに直交するように補正する
        // NormalMapのTBN行列を安定させるための処理
        l_worldTangent = normalize(l_worldTangent - (l_worldNormal * dot(l_worldTangent, l_worldNormal)));
        
        a_vertexList[l_vertexIndex].position      = mul(l_worldPosition, g_viewProjectionMatrix);
        a_vertexList[l_vertexIndex].worldPosition = l_worldPosition.xyz;
        a_vertexList[l_vertexIndex].worldNormal   = l_worldNormal;
        a_vertexList[l_vertexIndex].worldTangent  = float4(l_worldTangent, l_modelVertex.tangent.w);
        a_vertexList[l_vertexIndex].uv            = l_modelVertex.uv;
    }

    for (uint l_triangleIndex = 0U; l_triangleIndex < l_modelMeshlet.triangleCount; ++l_triangleIndex)
    {
        const uint l_primitiveIndex = l_modelMeshlet.triangleOffset + (l_triangleIndex * k_triangleVertexCount);
        
        a_primitiveList[l_triangleIndex] = uint3(l_primitiveIndexBuffer[l_primitiveIndex], l_primitiveIndexBuffer[l_primitiveIndex + k_secondPrimitiveVertexOffset], l_primitiveIndexBuffer[l_primitiveIndex + k_thirdPrimitiveVertexOffset]);
    }

}