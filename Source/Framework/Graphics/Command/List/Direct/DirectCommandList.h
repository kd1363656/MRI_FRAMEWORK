#pragma once

namespace FWK::Graphics
{
	class SwapChain;
	class RTVDescriptorHeap;
}

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		DirectCommandList (const Device& a_device);
		~DirectCommandList();

		void TransitionResource(const ComPtr<ID3D12Resource2>& a_resourece, const D3D12_RESOURCE_STATES a_beforState, const D3D12_RESOURCE_STATES a_afterState) const;

		void TransitionRenderTargetResource(const SwapChain& a_swapChain, const D3D12_RESOURCE_STATES a_beforState, const D3D12_RESOURCE_STATES a_afterState) const;

		void SetupRenderTarget(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const;

	private:

		static constexpr float k_clearColor[] = { 1.0F, 0.0F, 1.0F, 1.0F };

		static constexpr UINT k_executeClearRectNum    = 0U;
		static constexpr UINT k_sendBarrierNum		   = 1U;
		static constexpr UINT k_executeRenderTargetNum = 1U;
		static constexpr UINT k_setScissorRectNum      = 1U;
	};
}