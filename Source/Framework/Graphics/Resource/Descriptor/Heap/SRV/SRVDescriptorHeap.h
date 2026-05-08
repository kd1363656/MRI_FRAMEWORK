#pragma once

namespace FWK::Graphics
{
	class SRVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		 SRVDescriptorHeap();
		~SRVDescriptorHeap() override;

	private:

		static const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	};
}