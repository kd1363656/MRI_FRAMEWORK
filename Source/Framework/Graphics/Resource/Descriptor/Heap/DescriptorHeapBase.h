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

		bool Create(const UINT a_descriptorCapacity, const Device& a_device);

		bool CopyCPUOnlyDescriptorToShaderVisibleDescriptor(const UINT a_index, const Device& a_device) const;

		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALCPUOnlyCPUHandle      (const UINT a_index) const;
		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALShaderVisibleCPUHandle(const UINT a_index) const;
		
	private:

		bool CreateDescriptorHeapRecord(const D3D12_DESCRIPTOR_HEAP_FLAGS a_descriptorHeapFlag, const Device& a_device, DescriptorHeapRecord& a_descriptorHeapRecord) const;
		
		bool CreateDescriptorHeapRecordIfNeeded(const D3D12_DESCRIPTOR_HEAP_FLAGS            a_descriptorHeapFlag,
											    const Device&						           a_device, 
											    const bool							       a_shouldCreate,
											  	    std::shared_ptr<DescriptorHeapRecord>& a_descriptorHeapRecord) const;

		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALCPUHandle(const UINT a_index, const DescriptorHeapRecord& a_descriptorHeapRecord) const;

		static constexpr D3D12_DESCRIPTOR_HEAP_FLAGS k_cpuOnlyDescriptorHeapFlag       = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		static constexpr D3D12_DESCRIPTOR_HEAP_FLAGS k_shaderVisibleDescriptorHeapFlag = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		static constexpr D3D12_DESCRIPTOR_HEAP_TYPE  k_cbvSRVUAVDescriptorHeapType     = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		static constexpr D3D12_DESCRIPTOR_HEAP_TYPE  k_samplerDescriptorHeapType       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

		static constexpr UINT64 k_invalidGPUDescriptorHandlePTR = 0ULL;

		static constexpr UINT k_copyDescriptorCount         = 1U;
		static constexpr UINT k_uninitializedDescriptorSize = 0U;

		const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType;

		const bool k_isUseCPUOnly;
		const bool k_isUseShaderVisible;

		std::shared_ptr<DescriptorHeapRecord> m_cpuOnlyDescriptorHeapRecord       = nullptr;
		std::shared_ptr<DescriptorHeapRecord> m_shaderVisibleDescriptorHeapRecord = nullptr;

		UINT m_descriptorCapacity;
		UINT m_descriptorSize;
	};
}