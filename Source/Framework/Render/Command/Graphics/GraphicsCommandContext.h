#pragma once

namespace FWK::Render
{
	class GraphicsCommandContext
	{
	public:

		explicit GraphicsCommandContext(const Hardware& a_hardware);
		~GraphicsCommandContext        ();

		const auto& GetHardware() const { return k_hardware; }

	private:

		const Hardware& k_hardware;

		GraphicsCommandQueue m_graphicsCommandQueue;
	};
}