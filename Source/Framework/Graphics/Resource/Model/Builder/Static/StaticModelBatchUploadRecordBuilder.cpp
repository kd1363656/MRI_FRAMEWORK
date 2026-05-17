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
			ReleaseCreatedStaticModelStructuredBufferSRV(l_modelMeshList, a_srvDescriptorHeap);

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
								   l_modelMeshRuntimeData.m_vertexBuffer.m_bufferGPUResource))
	{
		assert(false && "ModelVertexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// メッシュレットバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_meshletList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_meshletBuffer.m_bufferGPUResource))
	{
		assert(false && "MeshletBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// ユニーク頂点インデックスバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_uniqueVertexIndexList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_bufferGPUResource))
	{
		assert(false && "UniqueVertexIndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// プリミティブインデックスバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_primitiveIndexList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_bufferGPUResource))
	{
		assert(false && "PrimitiveIndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// メッシュレットカリングバッファー用アップロードバッファの作成
	if (!CreateBufferUploadCommand(l_modelMeshletData.m_meshletBoundsList,
								   a_device,
								   a_gpuMemoryAllocator,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_bufferGPUResource))
	{
		assert(false && "MeshletBoundsBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// 頂点バッファー用SRVの作成
	l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID = CreateStructuredBufferSRV(a_modelMesh.m_modelVertexList,
																					 l_modelMeshRuntimeData.m_vertexBuffer.m_bufferGPUResource,
																					 a_device,
																					 a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID == Constant::k_invalidStorageID)
	{
		ReleaseCreatedModelMeshStructuredBufferSRV(l_modelMeshRuntimeData, a_srvDescriptorHeap);

		assert(false && "ModelVertexBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// メッシュレットバッファー用SRVの作成
	l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_meshletList,
																				      l_modelMeshRuntimeData.m_meshletBuffer.m_bufferGPUResource,
																				      a_device,
																				      a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID == Constant::k_invalidStorageID)
	{
		ReleaseCreatedModelMeshStructuredBufferSRV(l_modelMeshRuntimeData, a_srvDescriptorHeap);

		assert(false && "MeshletBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// ユニーク頂点インデックスバッファー用SRVの作成
	l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_uniqueVertexIndexList,
																							    l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_bufferGPUResource,
																							    a_device,
																							    a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID == Constant::k_invalidStorageID)
	{
		ReleaseCreatedModelMeshStructuredBufferSRV(l_modelMeshRuntimeData, a_srvDescriptorHeap);

		assert(false && "UniqueVertexIndexBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// プリミティブインデックスバッファー用SRVの作成
	l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_primitiveIndexList,
																						     l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_bufferGPUResource,
																						     a_device,
																						     a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID == Constant::k_invalidStorageID)
	{
		ReleaseCreatedModelMeshStructuredBufferSRV(l_modelMeshRuntimeData, a_srvDescriptorHeap);

		assert(false && "PrimitiveIndexBuffer用SRVの作成に失敗しました。");
		return false;
	}

	// メッシュレットカリング用SRVの作成
	l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_srvStorageID = CreateStructuredBufferSRV(l_modelMeshletData.m_meshletBoundsList,
																						    l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_bufferGPUResource,
																						    a_device,
																						    a_srvDescriptorHeap);

	if (l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_srvStorageID == Constant::k_invalidStorageID)
	{
		ReleaseCreatedModelMeshStructuredBufferSRV(l_modelMeshRuntimeData, a_srvDescriptorHeap);

		assert(false && "MeshletBoundsBuffer用SRVの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::StaticModelBatchUploadRecordBuilder::ReleaseCreatedStructuredBufferSRV(Struct::StructuredBufferResource& a_structuredBufferResource, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) const
{
	if (a_structuredBufferResource.m_srvStorageID == Constant::k_invalidStorageID) { return; }

	a_srvDescriptorPool.Release(a_structuredBufferResource.m_srvStorageID);

	a_structuredBufferResource.m_srvStorageID = Constant::k_invalidStorageID;
}
void FWK::Graphics::StaticModelBatchUploadRecordBuilder::ReleaseCreatedModelMeshStructuredBufferSRV(Struct::ModelMeshRuntimeData& a_modelMeshRuntimeData, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) const
{
	ReleaseCreatedStructuredBufferSRV(a_modelMeshRuntimeData.m_vertexBuffer,		    a_srvDescriptorPool);
	ReleaseCreatedStructuredBufferSRV(a_modelMeshRuntimeData.m_meshletBuffer,           a_srvDescriptorPool);
	ReleaseCreatedStructuredBufferSRV(a_modelMeshRuntimeData.m_uniqueVertexIndexBuffer, a_srvDescriptorPool);
	ReleaseCreatedStructuredBufferSRV(a_modelMeshRuntimeData.m_primitiveIndexBuffer,    a_srvDescriptorPool);
	ReleaseCreatedStructuredBufferSRV(a_modelMeshRuntimeData.m_meshletBoundsBuffer,     a_srvDescriptorPool);
}
void FWK::Graphics::StaticModelBatchUploadRecordBuilder::ReleaseCreatedStaticModelStructuredBufferSRV(std::vector<Struct::ModelMesh>& a_modelMeshList, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) const
{
	for (auto& l_modelMesh : a_modelMeshList)
	{	
		ReleaseCreatedModelMeshStructuredBufferSRV(l_modelMesh.m_modelMeshRuntimeData, a_srvDescriptorPool);
	}
}