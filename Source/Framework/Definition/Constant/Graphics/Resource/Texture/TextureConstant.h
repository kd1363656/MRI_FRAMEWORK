#pragma once

namespace FWK::Constant
{
	inline constexpr UINT k_emptyTextureReferenceCount = 0U;

	inline constexpr TypeAlias::TextureID k_invalidTextureID = std::numeric_limits<TypeAlias::TextureID>::max();

	inline constexpr TypeAlias::TextureID k_defaultCreateTextureIDCapacity = 3000U;
}