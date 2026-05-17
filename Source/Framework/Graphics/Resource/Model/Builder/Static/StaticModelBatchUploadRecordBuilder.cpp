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
	return false;
}