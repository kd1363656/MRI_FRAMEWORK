#pragma once

namespace FWK::Constant
{
	inline constexpr D3D12_RESOURCE_STATES k_defaultDepthStencilTextureResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	inline constexpr float k_defaultDepthClearValue = 1.0F;

	inline constexpr UINT k_invalidDepthStencilTextureWidth  = 0U;
	inline constexpr UINT k_invalidDepthStencilTextureHeight = 0U;

	inline constexpr UINT8 k_defaultStencilClearValue = 0U;
}