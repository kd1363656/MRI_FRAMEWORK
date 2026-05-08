#pragma once

namespace FWK::Graphics
{
	class RTVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		 RTVDescriptorHeap();
		~RTVDescriptorHeap() override;

	private:

		static const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	};
}