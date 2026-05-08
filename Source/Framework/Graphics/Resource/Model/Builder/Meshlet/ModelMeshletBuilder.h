#pragma once

namespace FWK::Graphics
{
	class ModelMeshletBuilder final
	{
	public:

		 ModelMeshletBuilder() = default;
		~ModelMeshletBuilder() = default;

		bool BuildStaticModelMeshletData(const Struct::StaticModelMesh& a_staticModelMesh, Struct::ModelMeshletData& a_modelMeshletData) const;

	private:

		static constexpr float k_meshletConeWeight = 0.0F;

		static constexpr std::size_t k_maxMeshletVertexCount   = 64ULL;
		static constexpr std::size_t k_maxMeshletTriangleCount = 124ULL;
		static constexpr std::size_t k_noIndexRemainder		   = 0ULL;
	};
}