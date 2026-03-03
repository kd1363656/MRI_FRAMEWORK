#pragma once

namespace FWK::Graphics
{
	class SwapChain;
}

namespace FWK::Graphics
{
	class ResourceContext final
	{
	public:

		ResourceContext (const Device& a_device);
		~ResourceContext();

		void Init  ();
		bool Create(const SwapChain& a_swapChain);

		const auto& GetDescriptorHeapContext() const { return m_descriptorHeapContext; }

	private:

		DescriptorHeapContext m_descriptorHeapContext;
	};
}