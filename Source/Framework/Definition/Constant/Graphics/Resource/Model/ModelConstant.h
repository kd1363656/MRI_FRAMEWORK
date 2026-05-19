#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::Math::Color k_defaultModelBaseColorFactor = { 1.0F, 1.0F, 1.0F, 1.0F };

	inline constexpr float k_defaultModelMetallicFactor  = 0.0F;
	inline constexpr float k_defaultModelRoughnessFactor = 1.0F;

	inline constexpr std::size_t k_emptyModelMeshCount = 0ULL;

	inline constexpr std::uint32_t k_disableModelTexture = 0U;
	inline constexpr std::uint32_t k_enableModelTexture  = 1U;

	inline constexpr std::uint32_t k_triangleVertexCount = 3U;
}