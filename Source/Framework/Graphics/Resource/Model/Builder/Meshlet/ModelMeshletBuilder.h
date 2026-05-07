#pragma once

namespace FWK::Graphics
{
	class ModelMeshletBuilder final
	{
	public:

		 ModelMeshletBuilder() = default;
		~ModelMeshletBuilder() = default;

		bool BuildStaticModelMeshletData(const Struct::StaticModelMesh& a_staticModelMesh, Struct::ModelMeshletData& a_modelMeshlettData) const;

	private:

		static constexpr std::size_t k_maxMEshletVertexCount   = 64ULL;
		static constexpr std::size_t k_maxMeshletTriangleCount = 124ULL;

		static constexpr float k_meshletConeWeight = 0.0F;
	};
}