#pragma once

namespace FWK::Graphics
{
	class SwapChain;
}

namespace FWK::Graphics
{
	class DescriptorHeapContext final
	{
	public:

		explicit DescriptorHeapContext(const Device& a_device);
		~DescriptorHeapContext        ();

		void Init  ();
		bool Create(const SwapChain& a_swapChain);

		const auto& GetRTVDescriptroHeap() const { return  m_rtvDescriptorHeap; }

	private:

		RTVDescriptorHeap m_rtvDescriptorHeap;
	};
}