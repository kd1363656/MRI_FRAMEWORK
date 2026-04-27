#pragma once

namespace FWK::Graphics
{
	class SRVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		 SRVDescriptorHeap();
		~SRVDescriptorHeap() override;

	private:

		static constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_descriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		static constexpr bool k_isUseCPUOnly       = true;
		static constexpr bool k_isUseShaderVisible = true;
	};
}