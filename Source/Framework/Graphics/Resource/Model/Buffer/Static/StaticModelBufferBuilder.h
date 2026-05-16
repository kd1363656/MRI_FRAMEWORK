#pragma once

namespace FWK::Graphics
{
	class StaticModelBufferBuilder
	{
	public:

		 StaticModelBufferBuilder() = default;
		~StaticModelBufferBuilder() = default;

		bool AppendStaticModelRecordBufferUpload(const std::weak_ptr<Struct::StaticModelRecord>&	    a_staticModelRecord,
												 const Device&										    a_device,
												 const GPUMemoryAllocator&							    a_gpuMemoryAllocator,
													   std::vector<Struct::BufferUploadRecord>&		    a_bufferUploadRecordList,
													   std::vector<TypeAlias::ComPtr<ID3D12Resource2>>& a_destinationBufferList) const;
		
	private:

		bool AppendModelMeshBufferUpload(const Device&			                                a_device,
										 const GPUMemoryAllocator&                              a_gpuMemoryAllocator,
											   Struct::ModelMesh&                               a_modelMesh,
											   std::vector<Struct::BufferUploadRecord>&         a_bufferUploadRecordList,
											   std::vector<TypeAlias::ComPtr<ID3D12Resource2>>& a_destinationBufferList) const;

		bool CreateBufferUploadRecord(const Device&								             a_device,
									  const GPUMemoryAllocator&					             a_gpuMemoryAllocator,
									  const UINT64&								             a_bufferSize,
									  const void*								             a_sourceData,
											std::vector<Struct::BufferUploadRecord>&         a_bufferUploadRecordList,
											std::vector<TypeAlias::ComPtr<ID3D12Resource2>>& a_destinationBufferList,
									        TypeAlias::ComPtr<ID3D12Resource2>&              a_destinationBufferResource,
									        TypeAlias::ComPtr<D3D12MA::Allocation>&          a_destinationBufferAllocation);
	};
}