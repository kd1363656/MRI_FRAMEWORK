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

		// 派生クラスのコンストラクタで作るシェーダータイプやシェーダー可視性を決める設計
		explicit DescriptorHeapBase(const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType, const bool a_isUseCPUOnly, const bool a_isUseShaderVisible);
		virtual ~DescriptorHeapBase();

		bool Create(const UINT a_descriptorCapacity, const Device& a_device);

	private:

		bool CreateDescriptorHeapRecord(const D3D12_DESCRIPTOR_HEAP_FLAGS a_descriptorHeapFlag, const Device& a_device, DescriptorHeapRecord&					 a_descriptorHeapRecord);
		
		bool CreateDescriptorHeapRecord(const D3D12_DESCRIPTOR_HEAP_FLAGS            a_descriptorHeapFlag,
								        const Device&						         a_device, 
								        const bool							         a_shouldCreate,
											  std::shared_ptr<DescriptorHeapRecord>& a_descriptorHeapRecord);

		static constexpr UINT64 k_invalidDescriptorPTR = 0ULL;

		const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType;

		const bool k_isUseCPUOnly;
		const bool k_isUseShaderVisible;

		std::shared_ptr<DescriptorHeapRecord> m_cpuOnlyDescriptorHeapRecord       = nullptr;
		std::shared_ptr<DescriptorHeapRecord> m_shaderVisibleDescriptorHeapRecord = nullptr;

		UINT m_descriptorCapacity;
		UINT m_descriptorSize;
	};
}