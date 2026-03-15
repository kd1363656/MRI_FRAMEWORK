#pragma once

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		DirectCommandList();
		~DirectCommandList() override;

		void TransitionResource(const Microsoft::WRL::ComPtr<ID3D12Resource2>& a_resource, const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState) const;
		
	private:

		static constexpr float k_clearColor[] = { 1.0F, 0.0F, 1.0F, 1.0F };

		static constexpr UINT k_sendBarrierNum = 1U;

		static constexpr UINT k_executeClearRectNum    = 0U;
		static constexpr UINT k_executeRenderTargetNum = 1U;
		static constexpr UINT k_setDescriptorHeapNum   = 1U;

		static constexpr UINT k_setViewportNum    = 1U;
		static constexpr UINT k_setScissorRectNum = 1U;
	};
}