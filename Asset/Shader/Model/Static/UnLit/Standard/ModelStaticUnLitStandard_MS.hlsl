#include "../../../ModelCommon.hlsli"

struct StaticModelVertex
{
    float3 position;
    float3 normal;
    float4 tangent;
    float2 uv;
};

[outputtopology("triangle")]
[numthreads(k_meshShaderThreadCountX, k_meshShaderThreadCountY, k_meshShaderThreadCountZ)]
void main(uint3                   a_groupID : SV_GroupID, 
          out vertices MeshOutput a_vertexList   [k_maxMeshletVertexCount],
          out indices  uint3      a_primitiveList[k_maxMeshletPrimitiveCount])
{
    StructuredBuffer<StaticModelVertex> l_modelVertexBuffer       = ResourceDescriptorHeap[g_vertexBufferIndex];
    StructuredBuffer<ModelMeshlet>      l_modelMeshletBuffer      = ResourceDescriptorHeap[g_meshletBufferIndex];
    StructuredBuffer<uint>              l_uniqueVertexIndexBuffer = ResourceDescriptorHeap[g_uniqueVertexIndexBufferIndex];
    StructuredBuffer<uint>              l_primitiveIndexBuffer    = ResourceDescriptorHeap[g_primitiveIndexBufferIndex];
    
    const uint l_meshletIndex = a_groupID.x;
    
    const ModelMeshlet l_modelMeshlet = l_modelMeshletBuffer[l_meshletIndex];
    
    // SetMeshOutputCounts(出力頂点数、
    //                     出力三角形数);
    
    SetMeshOutputCounts(l_modelMeshlet.vertexCount, l_modelMeshlet.triangleCount);

    for (uint l_vertexIndex = 0U; l_vertexIndex < l_modelMeshlet.vertexCount; ++l_vertexIndex)
    {
        const uint l_modelVertexIndex = l_uniqueVertexIndexBuffer[l_modelMeshlet.vertexOffset + l_vertexIndex];
        
        const StaticModelVertex l_modelVertex = l_modelVertexBuffer[l_modelVertexIndex];
        
        const float4 l_localPosition = float4(l_modelVertex.position, k_modelPositionVectorElementW);
        const float4 l_worldPosition = mul(l_localPosition, g_worldMatrix);

        a_vertexList[l_vertexIndex].position = mul(l_worldPosition, g_viewProjectionMatrix);
        a_vertexList[l_vertexIndex].uv       = l_modelVertex.uv;  
    }
    
    for (uint l_triangleIndex = 0U; l_triangleIndex < l_modelMeshlet.triangleCount; ++l_triangleIndex)
    {
        const uint l_primitiveIndex = l_modelMeshlet.triangleOffset + (l_triangleIndex * k_triangleVertexCount);

        a_primitiveList[l_triangleIndex] = uint3(l_primitiveIndexBuffer[l_primitiveIndex], l_primitiveIndexBuffer[l_primitiveIndex + k_secondPrimitiveVertexOffset], l_primitiveIndexBuffer[l_primitiveIndex + k_thirdPrimitiveVertexOffset]);
    }
}