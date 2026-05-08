#pragma once

namespace FWK::Graphics
{
	class ModelMeshletBuilder final
	{
	public:

		 ModelMeshletBuilder() = default;
		~ModelMeshletBuilder() = default;

		bool BuildModelMeshletData(Struct::ModelMesh& a_modelMesh, Struct::ModelMeshletData& a_modelMeshletData) const;

	private:

		static constexpr float k_meshletConeWeight = 0.0F;

		static constexpr std::size_t k_maxMeshletVertexCount   = 64ULL;
		static constexpr std::size_t k_maxMeshletTriangleCount = 124ULL;
		static constexpr std::size_t k_noIndexRemainder		   = 0ULL;
		static constexpr std::size_t k_emptyMeshletCount	   = 0ULL;
		static constexpr std::size_t k_lastElementOffset	   = 1ULL;
	};
}