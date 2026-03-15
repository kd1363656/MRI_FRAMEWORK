#pragma once

namespace FWK::Graphics
{
	class RTVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		RTVDescriptorHeap();
		~RTVDescriptorHeap() override;
	};
}