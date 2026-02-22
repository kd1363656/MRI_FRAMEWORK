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

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;

		const Hardware& k_hardware;

		GraphicsCommandQueue m_graphicsCommandQueue;
	};
}