#pragma once

namespace FWK::Graphics
{
	class SRVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		 SRVDescriptorHeap();
		~SRVDescriptorHeap() override;
	};
}