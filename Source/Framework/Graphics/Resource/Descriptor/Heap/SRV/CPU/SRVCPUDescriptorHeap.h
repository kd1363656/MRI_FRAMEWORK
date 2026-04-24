#pragma once

namespace FWK::Graphics
{
	class SRVCPUDescriptorHeap final : public SRVDescriptorHeapBase
	{
	public:

		 SRVCPUDescriptorHeap();
		~SRVCPUDescriptorHeap() override;
	};
}