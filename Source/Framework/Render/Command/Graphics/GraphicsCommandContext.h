#pragma once

namespace FWK::Render
{
	class GraphicsCommandContext
	{
	public:

		explicit GraphicsCommandContext(const Hardware& a_hardware);
		~GraphicsCommandContext        ();

		void Init  ();
		bool Create();

		const auto& GetHardware() const { return k_hardware; }

		const auto& GetGraphicsCommandQueue() const { return m_graphicsCommandQueue; }

	private:

		const Hardware& k_hardware;

		GraphicsCommandQueue m_graphicsCommandQueue;
		GraphicsCommandList  m_graphicsCommandList;
	};
}