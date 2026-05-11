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

		bool ExtractModelData(const ufbx_scene* a_fbxScene, Struct::ModelData& a_modelData) const;
		bool ExtractModelMesh(const ufbx_mesh*  a_fbxMesh,  Struct::ModelMesh& a_modelMesh) const;

		static constexpr float k_uvCoordinateMax = 1.0F;

		static constexpr std::uint32_t k_triangleVertexCount = 3U;
	};
}