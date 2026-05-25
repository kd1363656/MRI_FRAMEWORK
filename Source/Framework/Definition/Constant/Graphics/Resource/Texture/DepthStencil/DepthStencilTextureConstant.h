#pragma once

namespace FWK::Constant
{
	inline constexpr D3D12_RESOURCE_STATES k_defaultDepthStencilTextureResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	inline constexpr DXGI_FORMAT k_defaultDepthStencilTextureFormat = DXGI_FORMAT_D32_FLOAT;

	inline constexpr UINT k_defaultDepthStencilTextureWidth  = 0U;
	inline constexpr UINT k_defaultDepthStencilTextureHeight = 0U;
}