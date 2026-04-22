#pragma once

namespace FWK::Graphics
{
	class DescriptorHeapBase
	{
	public:

		// 派生クラスのコンストラクタで作るシェーダータイプやシェーダー可視性を決める設計
		explicit DescriptorHeapBase(const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType, const bool a_isShaderVisibleSupported, const bool a_isShaderVisible);
		virtual ~DescriptorHeapBase();

		bool Create(const UINT a_descriptorCapacity, const Device& a_device);

		D3D12_CPU_DESCRIPTOR_HANDLE FetchVALCPUHandle(const UINT a_index) const;
		
	private:

		static constexpr UINT64 k_invalidDescriptorPTR = 0ULL;

		const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType;

		const bool k_isShaderVisibleSupported;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;

		UINT m_descriptorCapacity;
		UINT m_descriptorSize;

		TypeAlias::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;

		bool m_isShaderVisible;
	};
}