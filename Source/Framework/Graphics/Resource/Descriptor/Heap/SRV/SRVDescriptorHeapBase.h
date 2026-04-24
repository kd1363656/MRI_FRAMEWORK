#pragma once

namespace FWK::Graphics
{
	class SRVDescriptorHeapBase : public DescriptorHeapBase
	{
	public:

		explicit SRVDescriptorHeapBase(const bool a_isShaderVisible);
				~SRVDescriptorHeapBase() override;
	};
}