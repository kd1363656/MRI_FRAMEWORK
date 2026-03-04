#pragma once

namespace FWK::Graphics
{
	class SRVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		explicit SRVDescriptorHeap(const Device& a_device);
		~SRVDescriptorHeap        ()					   override;
	};
}