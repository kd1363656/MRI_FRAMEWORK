#pragma once

namespace FWK::Graphics
{
	class SwapChain final
	{
	public:

		SwapChain (const Hardware& a_hardware);
		~SwapChain();

	private:

		const Hardware& k_hardware;

	};
}