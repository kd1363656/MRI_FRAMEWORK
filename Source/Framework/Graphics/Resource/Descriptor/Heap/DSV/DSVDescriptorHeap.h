#pragma once

namespace FWK::Graphics
{
	class DSVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		 DSVDescriptorHeap();
		~DSVDescriptorHeap() override;

	private:

		static const D3D12_DESCRIPTOR_HEAP_TYPE k_createDescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	};
}