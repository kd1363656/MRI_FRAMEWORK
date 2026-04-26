#pragma once

namespace FWK::Graphics
{
	class SwapChain;
	class RTVDescriptorHeap;
	class RenderArea;
	class RootSignature;
	class PipelineState;
}

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		 DirectCommandList();
		~DirectCommandList() override;

		void TransitionResource            (const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState, const TypeAlias::ComPtr<ID3D12Resource2>& a_resource)  const;
		void TransitionRenderTargetResource(const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState, const SwapChain&						   a_swapChain) const;

		void SetupBackBuffer(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const;

		void SetupRenderArea   (const RenderArea&    a_renderArea)    const;
		void SetupRootSignature(const RootSignature* a_rootSignature) const;
		void SetupPipelineState(const PipelineState* a_pipelineState) const;

		void DispatchMesh(const UINT a_threadCountGroupX, const UINT a_threadCountGroupY, const UINT a_threadCountGroupZ) const;

	private:

		static constexpr UINT k_sendBarrierNum = 1U;

		static constexpr UINT k_executeRenderTargetNum = 1U;
		static constexpr UINT k_executeClearRectNum    = 0U;

		static constexpr UINT k_setViewportNum    = 1U;
		static constexpr UINT k_setScissorRectNum = 1U;

		static constexpr float k_clearColor[] = { 1.0F, 0.80F, 1.0F, 1.0F };
	};
}