#pragma once

namespace FWK::Graphics
{
	class StaticModelFBXLoader final : public FBXModelLoaderBase
	{
	public:

		 StaticModelFBXLoader()			 = default;
		~StaticModelFBXLoader() override = default;

		bool LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const;

	private:

		bool ExtractModelData(Struct::ModelData& a_modelData, const ufbx_scene* a_fbxScene) const;
		bool ExtractModelMesh(Struct::ModelMesh& a_modelMesh, const ufbx_mesh*  a_fbxMesh)  const;

		static constexpr float k_uvCoordinateMax = 1.0F;

		static constexpr std::uint32_t k_triangleVertexCount = 3U;
	};
}