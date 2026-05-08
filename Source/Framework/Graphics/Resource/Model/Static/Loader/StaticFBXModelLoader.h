#pragma once

namespace FWK::Graphics
{
	class StaticFBXModelLoader final : public FBXModelLoaderBase
	{
	public:

		 StaticFBXModelLoader()          = default;
		~StaticFBXModelLoader() override = default;

		bool LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const;

	private:

		bool ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::ModelData& a_modelData) const;
		
		bool ExtractMesh(FbxMesh* a_fbxMesh, Struct::ModelMesh& a_modelMesh) const;

		static constexpr int k_emptyPolygonCount		 =  0;
		static constexpr int k_emptyUVSetCount           =  0;
		static constexpr int k_firstUVSetIndex           =  0;
	};
}