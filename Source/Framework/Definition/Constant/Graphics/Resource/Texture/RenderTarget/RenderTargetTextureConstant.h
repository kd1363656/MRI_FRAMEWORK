#pragma once

namespace FWK::Constant
{
	inline constexpr D3D12_RESOURCE_STATES k_defaultRenderTargetTextureResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	inline constexpr UINT k_renderTextureDefaultSampleCount   = 1U;
	inline constexpr UINT k_renderTextureDefaultSampleQuality = 0U;

	inline constexpr UINT16 k_renderTextureDefaultArraySize = 1U;
	inline constexpr UINT16 k_renderTextureDefaultMIPLevels = 1U;

	inline constexpr UINT k_defaultRenderTextureWidth  = 0U;
	inline constexpr UINT k_defaultRenderTextureHeight = 0U;
}