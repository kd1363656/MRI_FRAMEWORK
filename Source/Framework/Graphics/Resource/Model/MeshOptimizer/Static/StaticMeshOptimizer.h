#pragma once

namespace FWK::Graphics
{
	class StaticModelMeshOptimizer final
	{
	public:

		 StaticModelMeshOptimizer() = default;
		~StaticModelMeshOptimizer() = default;

		bool OptimizeStaticModelRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord) const;

	private:

		bool OptimizeModelMesh(Struct::ModelMesh& a_modelMesh) const;

		static constexpr std::size_t k_invalidOptimizedVertexCount = 0ULL;
	};
}