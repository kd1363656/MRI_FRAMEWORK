#pragma once

namespace FWK::Graphics
{
	class StaticModelBatchUploadRecordBuilder final
	{
	public:

		 StaticModelBatchUploadRecordBuilder() = default;
		~StaticModelBatchUploadRecordBuilder() = default;

		bool CreateStaticModelBatchUploadRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord,
												const Device&									a_device,
												const GPUMemoryAllocator&					    a_gpuMemoryAllocator,
													  std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
													  DescriptorPool<SRVDescriptorHeap>&		a_srvDescriptorHeap) const;

	private:

		static constexpr UINT64 k_firstStructuredBufferElement = 0ULL;

		bool CreateModelBatchUploadRecord(const Device&									  a_device,
										  const GPUMemoryAllocator&						  a_gpuMemoryAllocator,
											    std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommmandList,
												DescriptorPool<SRVDescriptorHeap>&		  a_srvDescriptorHeap,
												Struct::ModelMesh&						  a_modelMesh);

		template <typename Type>
		bool CreateBufferUploadCommand(const std::vector<Type>&                        a_bufferList,
									   const Device&			                       a_device,
									   const GPUMemoryAllocator&                       a_gpuMemoryAllocator,
										     std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
											 TypeAlias::ComPtr<ID3D12Resource2>&       a_destinationBufferResource,
											 TypeAlias::ComPtr<D3D12MA::Allocation>&   a_destinationBufferAllocation) const
		{
			if (a_bufferList.empty()) 
			{
				assert(false && "BufferListが空のため、BufferUploadCommandの作成に失敗しました。");
				return false;
			}

			const auto& l_bufferSize = sizeof(Type) * a_bufferList.size();


		}
	};
}