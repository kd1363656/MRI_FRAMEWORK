#pragma once

namespace FWK::Graphics
{
	class DescriptorHeapBase
	{
	public:

		DescriptorHeapBase         (const Device& a_device, const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptroHeapType, const bool a_isShaderVisible);
		virtual ~DescriptorHeapBase();

		void Init  ();
		bool Create(const UINT a_capacity);

		D3D12_CPU_DESCRIPTOR_HANDLE FetchCPUHandle(const UINT a_index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE FetchGPUHandle(const UINT a_index) const;

		const auto& GetHeap() const { return m_descriptorHeap; }

		UINT GetCapacity() const { return m_capacity; }

	private:

		static constexpr UINT64 k_invalidDescriptorPtr = 0ULL;

		const Device&                    k_device;
		const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType;
		const bool						 k_isShaderVisible;

		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;

		UINT m_capacity;
		UINT m_descriptorSize;
	};
}