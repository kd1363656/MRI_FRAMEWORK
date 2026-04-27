#pragma once

namespace FWK::Graphics
{
	class RTVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		 RTVDescriptorHeap();
		~RTVDescriptorHeap() override;

	private:

		static constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_descriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		static constexpr bool k_isUseCPUOnly       = true;
		static constexpr bool k_isUseShaderVisible = true;
	};
}