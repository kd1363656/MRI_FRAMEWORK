#pragma once

namespace FWK::Constant
{
	inline constexpr float k_meshletConeWeight = 0.0F;

	inline constexpr float k_meshletBoundsPadding = 0.0F;

	inline constexpr float k_initialMeshletBoundsRadius     = 0.0F;
	inline constexpr float k_initialMeshletBoundsConeCutoff = 0.0F;
	inline constexpr float k_initialMeshletBoundsPadding    = 0.0F;

	inline constexpr std::size_t k_emptyModelVertexCount = 0ULL;
	inline constexpr std::size_t k_emptyModelIndexCount  = 0ULL;
	inline constexpr std::size_t k_emptyMeshletCount     = 0ULL;

	inline constexpr std::size_t k_emptyUniqueVertexIndexCount = 0ULL;
	inline constexpr std::size_t k_emptyPrimitiveIndexCount    = 0ULL;
	inline constexpr std::size_t k_emptyMeshletBoundsCount     = 0ULL;

	inline constexpr std::size_t k_meshletPrimitiveIndexAlignment = 4ULL;

	inline constexpr std::uint32_t k_maxMeshletVertexCount    = 64U;
	inline constexpr std::uint32_t k_maxMeshletPrimitiveCount = 126U;

	inline constexpr std::uint32_t k_initialMeshletVertexOffset   = 0U;
	inline constexpr std::uint32_t k_initialMeshletTriangleOffset = 0U;
	inline constexpr std::uint32_t k_initialMeshletVertexCount    = 0U;
	inline constexpr std::uint32_t k_initialMeshletTriangleCount  = 0U;
}