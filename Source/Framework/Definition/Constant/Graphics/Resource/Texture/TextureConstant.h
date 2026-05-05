#pragma once

namespace FWK::Constant
{
	inline constexpr UINT k_emptyTextureReferenceCount = 0U;

	inline constexpr UINT k_defaultSpriteRectX      = 0U;
	inline constexpr UINT k_defaultSpriteRectY      = 0U;
	inline constexpr UINT k_defaultSpriteRectWidth  = 0U;
	inline constexpr UINT k_defaultSpriteRectHeight = 0U;

	inline constexpr TypeAlias::Math::Color k_noColor = { 0.0F, 0.0F, 0.0F, 1.0F };

	inline constexpr TypeAlias::Math::Vector2 k_defaultPivot = { 0.5F, 0.5F };

	inline constexpr TypeAlias::TextureID k_invalidTextureID = std::numeric_limits<TypeAlias::TextureID>::max();

	inline constexpr TypeAlias::TextureID k_defaultCreateTextureIDCapacity = 3000U;
}