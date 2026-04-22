#pragma once

namespace FWK::Graphics
{
	class RenderArea final
	{
	public:

		 RenderArea() = default;
		~RenderArea() = default;

		bool SetupRenderArea(const SwapChain& a_swapChain);

		const auto& GetREFViewport() const { return m_viewport; }
		const auto& GetScissorRect() const { return m_scissorRect; }

	private:

		static constexpr LONG k_defaultScissorRectLeft = 0L;
		static constexpr LONG k_defaultScissorRectTop  = 0L;

		D3D12_VIEWPORT m_viewport    = {};
		D3D12_RECT     m_scissorRect = {};
	};
}