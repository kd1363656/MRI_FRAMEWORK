#pragma once

namespace FWK::Graphics
{
	class StaticModelBatchUploadRecordBuilder final
	{
	public:

		 StaticModelBatchUploadRecordBuilder() = default;
		~StaticModelBatchUploadRecordBuilder() = default;

		bool CreateStaticModelBatchUploadRecord(const Device&									a_device,
												const GPUMemoryAllocator&					    a_gpuMemoryAllocator,
													  std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
													  DescriptorPool<SRVDescriptorHeap>&		a_srvDescriptorHeap,
													  Struct::StaticModelRecord&			    a_staticModelRecord) const;

	private:

		static constexpr UINT64 k_firstStructuredBufferElement = 0ULL;

		static constexpr UINT64 k_maxStructuredBufferElementCount = std::numeric_limits<UINT>::max();

		bool CreateModelBatchUploadRecord(const Device&									  a_device,
										  const GPUMemoryAllocator&						  a_gpuMemoryAllocator,
											    std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
												DescriptorPool<SRVDescriptorHeap>&		  a_srvDescriptorHeap,
												Struct::ModelMesh&						  a_modelMesh) const;

		void ReleaseCreatedStructuredBufferSRV           (Struct::StructuredBufferResource& a_structuredBufferResource, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) const;
		void ReleaseCreatedModelMeshStructuredBufferSRV  (Struct::ModelMeshRuntimeData&	    a_modelMeshRuntimeData,     DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) const;
		void ReleaseCreatedStaticModelStructuredBufferSRV(std::vector<Struct::ModelMesh>&   a_modelMeshList,		    DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) const;

		template <typename Type>
		bool CreateBufferUploadCommand(const std::vector<Type>&                        a_bufferList,
									   const Device&			                       a_device,
									   const GPUMemoryAllocator&                       a_gpuMemoryAllocator,
										     std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList,
											 Struct::GPUResource&					   a_destinationBufferGPUResource) const
		{
			if (a_bufferList.empty()) 
			{
				assert(false && "BufferListが空のため、BufferUploadCommandの作成に失敗しました。");
				return false;
			}

			const auto& l_bufferSize = sizeof(Type) * a_bufferList.size();

			// DEFAULTヒープ上に、MeshShaderから参照する本番用BufferResourceを作成する
			// 初期状態をCOMMONにしておくことでCopyQueue上のCopyBufferRegion時にCOPY_DESTへ暗黙的に昇格できる
			if (!a_gpuMemoryAllocator.CreateBufferResource(l_bufferSize, D3D12_RESOURCE_STATE_COMMON, a_destinationBufferGPUResource))
			{
				assert(false && "StaticModel用BufferResourceの作成に失敗したため、BufferUploadCommandの作成に失敗しました。");
				return false;
			}

			Struct::BufferUploadCommand l_bufferUploadCommand = {};

			l_bufferUploadCommand.m_destinationBufferResource = a_destinationBufferGPUResource.m_resource;
			
			auto& l_bufferUploadRecord = l_bufferUploadCommand.m_bufferUploadRecord;

			l_bufferUploadRecord.m_bufferSize = l_bufferSize;

			// DEFAULTヒープ上のBufferResourceへコピーするためのUploadBufferを作成する
			if (!l_bufferUploadRecord.m_uploadBuffer.Create(a_device, l_bufferSize))
			{
				assert(false && "StaticModel用UploadBufferの作成に失敗したため、BufferUploadCommandの作成に失敗しました。");
				return false;
			}

			auto* l_mappedData = l_bufferUploadRecord.m_uploadBuffer.Map();

			if (!l_mappedData)
			{
				assert(false && "StaticModel用UploadBufferのMapに失敗したため、BufferUploadCommandの作成に失敗しました。");
				return false;
			}

			std::memcpy(l_mappedData, a_bufferList.data(), l_bufferSize);

			l_bufferUploadRecord.m_uploadBuffer.UnMap();

			a_bufferUploadCommandList.emplace_back(std::move(l_bufferUploadCommand));

			return true;
		}

		template <typename Type>
		TypeAlias::StorageID CreateStructuredBufferSRV(const std::vector<Type>&					 a_bufferList,
													   const Struct::GPUResource&				 a_bufferGPUResource,
													   const Device&							 a_device,
															 DescriptorPool<SRVDescriptorHeap>&  a_srvDescriptorHeap) const
		{	
			const auto& l_device = a_device.GetREFDevice();

			if (!l_device)
			{
				assert(false && "デバイスが作成されておらず、StructuredBuffer用SRVの作成に失敗しました。");
				return Constant::k_invalidStorageID;
			}

			if (!a_bufferGPUResource.m_resource)
			{
				assert(false && "BufferResourceが無効のため、StructuredBuffer用SRVの作成に失敗しました。");
				return Constant::k_invalidStorageID;
			}

			if (a_bufferList.empty())
			{
				assert(false && "BufferListが空のため、StructuredBuffer用SRVの作成に失敗しました。");
				return Constant::k_invalidStorageID;
			}

			if (a_bufferList.size() > k_maxStructuredBufferElementCount)
			{
				assert(false && "StructuredBufferの要素数がUINTの最大値を超えたため、StructuredBuffer用SRVの作成に失敗しました。");
				return Constant::k_invalidStorageID;
			}
			
			const auto l_srvStorageID = a_srvDescriptorHeap.Allocate();

			if (l_srvStorageID == Constant::k_invalidStorageID)
			{
				assert(false && "SRV用StorageIDの確保に失敗したため、StructuredBuffer用のSRVの作成に失敗しました。");
				return Constant::k_invalidStorageID;
			}

			D3D12_SHADER_RESOURCE_VIEW_DESC l_srvDesc = {};

			// D3D12_SHADER_RESOURCE_VIEW_DESCについて
			// Shader4ComponentMapping : Shader側でRGBA部分をどのように読むか
			// Format                  : StructuredBufferなのでDXGI_FORMAT_UNKNOWNを指定する
			// ViewDimension		   : BufferをSRVとして参照するためBUFFERを指定する
			l_srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			l_srvDesc.Format				  = DXGI_FORMAT_UNKNOWN;
			l_srvDesc.ViewDimension			  = D3D12_SRV_DIMENSION_BUFFER;

			// D3D12_BUFFER_SRVについて
			// FirstElement         : 先頭要素番号
			// NumElements          : StructuredBufferとして参照する要素数
			// StructureByteStride  : 1要素あたりのバイトサイズ
			// Flags				: RawBufferではないためNONEを指定する
			l_srvDesc.Buffer.FirstElement        = k_firstStructuredBufferElement;
			l_srvDesc.Buffer.NumElements         = static_cast<UINT>(a_bufferList.size());
			l_srvDesc.Buffer.StructureByteStride = sizeof(Type);
			l_srvDesc.Buffer.Flags				 = D3D12_BUFFER_SRV_FLAG_NONE;

			const auto l_cpuOnlyCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(a_srvDescriptorHeap.FetchVALCPUOnlyCPUHandle(l_srvStorageID));

			// CreateShaderResourceView(BufferResource, 
			//							SRV設定、
			//							CPUOnlyDescriptorHeap側のCPUHandle);
			l_device->CreateShaderResourceView(a_bufferGPUResource.m_resource.Get(), &l_srvDesc, l_cpuOnlyCPUHandle);

			if (!a_srvDescriptorHeap.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(l_srvStorageID, a_device))
			{
				a_srvDescriptorHeap.Release(l_srvStorageID);

				assert(false && "CPUOnlyからShaderVisibleSRVへのコピーに失敗したため、StructuredBuffer用SRVの作成に失敗しました。");
				return Constant::k_invalidStorageID;
			}

			return l_srvStorageID;
		}
	};
}