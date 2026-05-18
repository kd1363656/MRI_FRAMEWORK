#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::Math::Color k_noColor = { 0.0F, 0.0F, 0.0F, 1.0F };

	inline constexpr TypeAlias::Math::Vector2 k_defaultPivot = { 0.5F, 0.5F };

	inline constexpr float k_defaultDepthClearValuie = 1.0F;

	inline constexpr UINT k_defaultSpriteRECTX      = 0U;
	inline constexpr UINT k_defaultSpriteRECTY      = 0U;
	inline constexpr UINT k_defaultSpriteRECTWidth  = 0U;
	inline constexpr UINT k_defaultSpriteRECTHeight = 0U;

	inline constexpr UINT k_renderTextureDefaultSampleCount   = 1U;
	inline constexpr UINT k_renderTextureDefaultSampleQuality = 0U;

	inline constexpr UINT16 k_renderTextureDefaultArraySize = 1U;
	inline constexpr UINT16 k_renderTextureDefaultMIPLevels = 1U;

	inline constexpr UINT8 k_defaultStencilClearValue = 0U;
}