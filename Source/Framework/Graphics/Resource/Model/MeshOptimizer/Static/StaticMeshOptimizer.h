#pragma once

namespace FWK::Graphics
{
	class StaticModelMeshOptimizer final
	{
	public:

		 StaticModelMeshOptimizer() = default;
		~StaticModelMeshOptimizer() = default;

		bool OptimizeModelData(Struct::ModelData& a_modelData) const;

	private:

		bool OptimizeModelMesh(Struct::ModelMesh& a_modelMesh) const;

		static constexpr std::size_t k_invalidOptimizedVertexCount = 0ULL;
	};
}