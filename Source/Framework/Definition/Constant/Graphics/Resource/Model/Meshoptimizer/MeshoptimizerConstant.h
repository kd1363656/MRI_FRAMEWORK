#pragma once

namespace FWK::Constant
{
	inline constexpr float k_meshletConeWeight = 0.0F;

	inline constexpr float k_meshletBoundsPadding = 0.0F;

	inline constexpr std::size_t k_emptyModelVertexCount = 0ULL;
	inline constexpr std::size_t k_emptyModelIndexCount  = 0ULL;
	inline constexpr std::size_t k_emptyMeshletCount     = 0ULL;

	inline constexpr std::size_t k_emptyUniqueVertexIndexCount = 0ULL;
	inline constexpr std::size_t k_emptyPrimitiveIndexCount    = 0ULL;
	inline constexpr std::size_t k_emptyMeshletBoundsCount     = 0ULL;

	inline constexpr std::size_t k_meshletPrimitiveIndexAlignment = 4ULL;

	inline constexpr std::uint32_t k_triangleVertexCount = 3U;
	
	inline constexpr std::uint32_t k_maxMeshletVertexCount    = 64U;
	inline constexpr std::uint32_t k_maxMeshletPrimitiveCount = 126U;
}