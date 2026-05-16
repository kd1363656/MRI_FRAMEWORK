#pragma once

namespace FWK::Graphics
{
	class StaticModelBufferBuilder
	{
	public:

		 StaticModelBufferBuilder() = default;
		~StaticModelBufferBuilder() = default;

		bool CreateStaticModelRecordBufferUpload(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord,
												 const Device&									 a_device,
												 const GPUMemoryAllocator&						 a_gpuMemoryAllocator,
													   std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList) const;
		
	private:

		template<typename Type>
		bool CreateBufferUploadCommand(const Device&								   a_device,
									   const GPUMemoryAllocator&					   a_gpuMemoryAllocator,
									   const std::vector<Type>&						   a_bufferList,
											 std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
									         TypeAlias::ComPtr<ID3D12Resource2>&       a_destinationBufferResource,
									         TypeAlias::ComPtr<D3D12MA::Allocation>&   a_destinationBufferAllocation) const
		{
			if (a_bufferList.empty())
			{
				assert(false && "BufferListが空のため、BufferUploadCommandの作成に失敗しました。");
				return false;
			}

			const auto l_bufferSize = sizeof(Type) * a_bufferList.size();

			if(!CreateBufferUploadCommand(a_device,
									      a_gpuMemoryAllocator,
									      l_bufferSize,
									      a_bufferList.data(),
									      a_bufferUploadCommandList,
									      a_destinationBufferResource,
									      a_destinationBufferAllocation))
			{
				assert(false && "バッファアップロード用リソースの作成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CreateModelMeshBufferUpload(const Device&			                         a_device,
										 const GPUMemoryAllocator&                       a_gpuMemoryAllocator,
											   Struct::ModelMesh&                        a_modelMesh,
											   std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList) const;

		bool CreateBufferUploadCommand(const Device&								   a_device,
									   const GPUMemoryAllocator&					   a_gpuMemoryAllocator,
									   const UINT64&								   a_bufferSize,
									   const void*								       a_sourceData,
											 std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
									         TypeAlias::ComPtr<ID3D12Resource2>&       a_destinationBufferResource,
									         TypeAlias::ComPtr<D3D12MA::Allocation>&   a_destinationBufferAllocation) const;
	};
}