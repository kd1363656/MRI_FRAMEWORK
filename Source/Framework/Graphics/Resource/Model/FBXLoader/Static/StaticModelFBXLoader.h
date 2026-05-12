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

		void ExtractModelMaterial(const ufbx_mesh* a_fbxMesh, Struct::ModelMaterial& a_modelMaterial) const;

		std::wstring FetchTextureFilePath      (const ufbx_material_map& a_materialMap) const;
		std::wstring ConvertUFBXStringToWString(const ufbx_string&		 a_fbxString)   const;

		static constexpr float k_uvCoordinateMax = 1.0F;

		static constexpr std::size_t k_firstMaterialIndex = 0ULL;
		static constexpr std::size_t k_emptyStringLength  = 0ULL;

		static constexpr std::uint32_t k_triangleVertexCount = 3U;
	};
}