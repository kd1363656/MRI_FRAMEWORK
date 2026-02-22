#pragma once

namespace FWK::Render
{
	class DescriptorHeap final
	{
	public:

		explicit DescriptorHeap(const Hardware& a_hardware, const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType);
		~DescriptorHeap        ();

		void Init();

		bool Create(const std::uint32_t a_numDescriptors, const bool a_shaderVisible);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const std::uint32_t a_index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const std::uint32_t a_index) const;

	private:

		static constexpr UINT64 k_invalidDescriptorPtr = 0ULL;

		const Hardware&					 k_hardware;
		const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType;

		TypeDEF::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap = nullptr;


		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;

		std::uint32_t m_descriptorSize;
		std::uint32_t m_capacity;
	};
}