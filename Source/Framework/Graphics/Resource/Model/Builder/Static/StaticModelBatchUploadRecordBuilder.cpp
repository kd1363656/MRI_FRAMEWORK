#include "StaticModelBatchUploadRecordBuilder.h"

bool FWK::Graphics::StaticModelBatchUploadRecordBuilder::CreateStaticModelBatchUploadRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, 
																							const Device&									a_device, 
																						    const GPUMemoryAllocator&						a_gpuMemoryAllocator, 
																								  std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
																								  DescriptorPool<SRVDescriptorHeap>&		a_srvDescriptorHeap) const
{
	return false;
}

bool FWK::Graphics::StaticModelBatchUploadRecordBuilder::CreateModelBatchUploadRecord(const Device&				                      a_device, 
																					  const GPUMemoryAllocator&                       a_gpuMemoryAllocator, 
																							std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommmandList, 
																							DescriptorPool<SRVDescriptorHeap>&		  a_srvDescriptorHeap, 
																							Struct::ModelMesh&						  a_modelMesh)
{
	return false;
}
