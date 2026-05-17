#pragma once

namespace FWK::Graphics
{
	class StaticModelBatchUploadRecordBuilder final
	{
	public:

		 StaticModelBatchUploadRecordBuilder() = default;
		~StaticModelBatchUploadRecordBuilder() = default;

		bool CreateStaticModelBatchUploadRecord(const Device&			                 a_device,
												const GPUMemoryAllocator&                a_gpuMemoryAllocator,
												const Struct::ModelData&                 a_modelData,
													  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
													  Struct::StaticModelRecord&		 a_staticModelRecord) const;

	private:

		static constexpr UINT64 k_firstStructuredBufferElement = 0ULL;

		bool CreateModelMeshBatchUploadRecord(const Device&									  a_device,
											  const GPUMemoryAllocator&						  a_gpuMemoryAllocator,
											  const Struct::ModelMesh&						  a_modelMesh,
													std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
													DescriptorPool<SRVDescriptorHeap>&        a_srvDescriptorPool,
												    Struct::ModelMeshRuntimeData&             a_modelMeshRuntimeData);
	};
}