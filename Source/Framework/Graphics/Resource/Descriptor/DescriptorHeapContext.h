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

		const auto& GetRTVDescriptorHeap() const { return m_rtvDescriptorHeap; }
		const auto& GetSRVDescriptorHeap() const { return m_srvDescriptorHeap; }

	private:

		static constexpr UINT k_defaultSrvHeapCapacity = 1024U;

		RTVDescriptorHeap m_rtvDescriptorHeap;
		SRVDescriptorHeap m_srvDescriptorHeap;
	};
}