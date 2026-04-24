#pragma once

namespace FWK::Graphics
{
	class SRVShaderVisibleDescriptorHeap final : public SRVDescriptorHeapBase
	{
	public:

		 SRVShaderVisibleDescriptorHeap();
		~SRVShaderVisibleDescriptorHeap() override;
	};
}