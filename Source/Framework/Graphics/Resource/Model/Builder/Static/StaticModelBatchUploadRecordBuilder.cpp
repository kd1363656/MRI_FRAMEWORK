#include "StaticModelBatchUploadRecordBuilder.h"

bool FWK::Graphics::StaticModelBatchUploadRecordBuilder::CreateStaticModelBatchUploadRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, 
																							const Device&									a_device, 
																						    const GPUMemoryAllocator&						a_gpuMemoryAllocator, 
																								  std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
																								  DescriptorPool<SRVDescriptorHeap>&		a_srvDescriptorHeap) const
{
	const auto l_staticModelRecord = a_staticModelRecord.lock();

	if (!l_staticModelRecord)
	{
		assert(false && "StaticModelRecordが無効のため、StaticModelBatchUploadRecordの作成に失敗しました。");
		return false;
	}

	auto& l_modelMeshList = l_staticModelRecord->m_modelData.m_modelMeshList;

	if (l_modelMeshList.empty())
	{
		assert(false && "ModelMeshListが空のため、StaticModelBatchUploadの作成に失敗しました。");
		return false;
	}

	for (auto& l_modelMesh : l_modelMeshList)
	{
		if (!CreateModelBatchUploadRecord(a_device,
										  a_gpuMemoryAllocator,
										  a_bufferUploadCommandList,
										  a_srvDescriptorHeap,
										  l_modelMesh))
		{
			assert(false && "ModelMesh用BatchUploadRecordの作成に失敗したため、StaticModelBatchUploadRecordの作成に失敗しました。");
			return false;
		}
	}

	return true;
}

bool FWK::Graphics::StaticModelBatchUploadRecordBuilder::CreateModelBatchUploadRecord(const Device&									  a_device,
																					  const GPUMemoryAllocator&						  a_gpuMemoryAllocator, 
																							std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList, 
																							DescriptorPool<SRVDescriptorHeap>&		  a_srvDescriptorHeap, 
																							Struct::ModelMesh&						  a_modelMesh) const
{
	const auto& l_modelMeshletData     = a_modelMesh.m_modelMeshletData;
		  auto& l_modelMeshRuntimeData = a_modelMesh.m_modelMeshRuntimeData;

	// 頂点バッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(a_modelMesh.m_modelVertexList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_vertexBufferResource,
								   l_modelMeshRuntimeData.m_vertexBufferAllocation))
	{
		assert(false && "ModelVertexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// メッシュレットバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_meshletList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_meshletBufferResource,
								   l_modelMeshRuntimeData.m_meshletBufferAllocation))
	{
		assert(false && "MeshletBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// ユニーク頂点インデックスバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_uniqueVertexIndexList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_uniqueVertexIndexBufferResource,
								   l_modelMeshRuntimeData.m_uniqueVertexIndexBufferAllocation))
	{
		assert(false && "UniqueVertexIndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// プリミティブインデックスバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_primitiveIndexList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_primitiveIndexBufferResource,
								   l_modelMeshRuntimeData.m_primitiveIndexBufferAllocation))
	{
		assert(false && "PrimitiveIndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// メッシュレットカリングバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_meshletBoundsList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_meshletBoundsBufferResource,
								   l_modelMeshRuntimeData.m_meshletBoundsBufferAllocation))
	{
		assert(false && "MeshletBoundsBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// 頂点バッファー用SRVの作成
	l_modelMeshRuntimeData.m_vertexBufferSRVStorageID = CreateStructuredBufferSRV(a_modelMesh.m_modelVertexList,
																				  l_modelMeshRuntimeData.m_vertexBufferResource,
																				  a_device,
																				  a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_vertexBufferSRVStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "ModelVertexBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// メッシュレットバッファー用SRVの作成
	l_modelMeshRuntimeData.m_meshletBufferSRVStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_meshletList,
																				   l_modelMeshRuntimeData.m_meshletBufferResource,
																				   a_device,
																				   a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_meshletBufferSRVStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "MeshletBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// ユニーク頂点インデックスバッファー用SRVの作成
	l_modelMeshRuntimeData.m_uniqueVertexIndexBufferSRVStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_uniqueVertexIndexList,
																							 l_modelMeshRuntimeData.m_uniqueVertexIndexBufferResource,
																							 a_device,
																							 a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_uniqueVertexIndexBufferSRVStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "UniqueVertexIndexBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// プリミティブインデックスバッファー用SRVの作成
	l_modelMeshRuntimeData.m_primitiveIndexBufferSRVStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_primitiveIndexList,
																						  l_modelMeshRuntimeData.m_primitiveIndexBufferResource,
																						  a_device,
																						  a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_primitiveIndexBufferSRVStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "PrimitiveIndexBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// メッシュレットカリング用SRVの作成
	l_modelMeshRuntimeData.m_meshletBoundsBufferSRVStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_meshletBoundsList,
																						 l_modelMeshRuntimeData.m_meshletBoundsBufferResource,
																						 a_device,
																						 a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_meshletBoundsBufferSRVStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "MeshletBoundsBuffer用SRVの作成に失敗しました。");
		return false;
	}

	return true;
}