#pragma once

namespace FWK::Graphics
{
	class RenderArea final
	{
	public:

		RenderArea () = default;
		~RenderArea() = default;

		void Init           ();
		bool SetupRenderArea(const SwapChain& a_swapChain);

		const auto& GetViewport   () const { return m_viewport; }
		const auto& GetScissorRect() const { return m_scissorRect; }

	private:

		static constexpr float k_defaultViewPortTopLeftX = 0.0F;
		static constexpr float k_defaultViewPortTopLeftY = 0.0F;

		static constexpr LONG k_defaultScissorRectLeft = 0;
		static constexpr LONG k_defaultScissorRectTop  = 0;

		D3D12_VIEWPORT m_viewport    = D3D12_VIEWPORT();
		D3D12_RECT     m_scissorRect = D3D12_RECT    ();
	};
}