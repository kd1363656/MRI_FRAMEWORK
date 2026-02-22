#pragma once

namespace FWK::Render
{
	class GraphicsCommandList final : public CommandListBase
	{
	public:

		explicit GraphicsCommandList(const GraphicsCommandContext& a_graphicsCommandContext);
		~GraphicsCommandList        () override;

		void Reset(const TypeDEF::ComPtr<ID3D12CommandAllocator>& a_commandAllocator) const;

		void TransitionResource(const TypeDEF::ComPtr<ID3D12Resource2>& a_resource,
								const D3D12_RESOURCE_STATES             a_beforState,
								const D3D12_RESOURCE_STATES			    a_afterState,
								const UINT							    a_sendResourceBarrierNum) const;

		void SetUpRenderTarget(const SwapChain& a_swapChain, const UINT a_executeRenderTargetNum) const;

		void Close() const;

	private:

		static constexpr float k_clearColor[] = { 1.0F, 0.0F, 1.0F, 1.0F };

		static constexpr UINT k_executeClearRectNum = 0U;

		const GraphicsCommandContext& k_graphicsCommandContext;
	};
}