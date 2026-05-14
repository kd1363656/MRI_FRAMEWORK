#pragma once

namespace FWK::Graphics
{
	class DescriptorHeapBase
	{
	private:

		struct DescriptorHeapRecord final
		{
			TypeAlias::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap = nullptr;

			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart = {};
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart = {};
		};

	public:

		// 派生クラスのコンストラクタで作るディスクリプタヒープタイプやシェーダー可視性を決める設計
		explicit DescriptorHeapBase(const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType, const bool a_isUseCPUOnly, const bool a_isUseShaderVisible);
		virtual ~DescriptorHeapBase();

		bool Create(const Device& a_device, const TypeAlias::StorageID a_storageIDCapacity);

		bool CopyCPUOnlyDescriptorToShaderVisibleDescriptor(const TypeAlias::StorageID a_storageID, const Device& a_device) const;

		TypeAlias::ComPtr<ID3D12DescriptorHeap> FetchPTRShaderVisibleDescriptorHeap() const;

		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALCPUOnlyCPUHandle      (const TypeAlias::StorageID a_storageID) const;
		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALShaderVisibleCPUHandle(const TypeAlias::StorageID a_storageID) const;
		
		D3D12_GPU_DESCRIPTOR_HANDLE FetchVALShaderVisibleGPUHandle(const TypeAlias::StorageID a_storageID) const;

	private:

		bool CreateDescriptorHeapRecord(const Device& a_device, const D3D12_DESCRIPTOR_HEAP_FLAGS a_descriptorHeapFlag, DescriptorHeapRecord& a_descriptorHeapRecord) const;
		
		bool CreateDescriptorHeapRecordIfNeeded(const Device&								 a_device, 
												const D3D12_DESCRIPTOR_HEAP_FLAGS            a_descriptorHeapFlag, 
											    const bool							         a_shouldCreate,
											  	      std::shared_ptr<DescriptorHeapRecord>& a_descriptorHeapRecord) const;

		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALCPUHandle(const TypeAlias::StorageID a_storageID, const DescriptorHeapRecord& a_descriptorHeapRecord) const;
		D3D12_GPU_DESCRIPTOR_HANDLE FetchVALGPUHandle(const TypeAlias::StorageID a_storageID, const DescriptorHeapRecord& a_descriptorHeapRecord) const;

		static constexpr UINT64 k_invalidGPUDescriptorHandlePTR = 0ULL;

		static constexpr UINT k_copyDescriptorCount         = 1U;
		static constexpr UINT k_uninitializedDescriptorSize = 0U;

		const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType;

		const bool k_isUseCPUOnly;
		const bool k_isUseShaderVisible;

		std::shared_ptr<DescriptorHeapRecord> m_cpuOnlyDescriptorHeapRecord;
		std::shared_ptr<DescriptorHeapRecord> m_shaderVisibleDescriptorHeapRecord;

		TypeAlias::StorageID m_descriptorStorageIDCapacity;
		UINT				 m_descriptorSize;
	};
}