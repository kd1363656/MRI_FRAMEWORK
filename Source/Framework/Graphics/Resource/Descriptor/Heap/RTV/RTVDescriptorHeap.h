#pragma once

namespace FWK::Graphics
{
	class RTVDescriptorHeap final : public DescriptorHeapBase
	{
	public:

		explicit RTVDescriptorHeap(const Device& a_device);
		~RTVDescriptorHeap        ()					   override;
	};
}