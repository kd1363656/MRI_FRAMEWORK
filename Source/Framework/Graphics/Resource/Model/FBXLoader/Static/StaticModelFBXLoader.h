#pragma once

namespace FWK::Graphics
{
	class StaticModelFBXLoader final : public FBXLoaderBase
	{
	public:

		 StaticModelFBXLoader()			 = default;
		~StaticModelFBXLoader() override = default;

		bool LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::StaticModelData& a_staticModelData, FbxManager* a_fbxManager) const;

	private:

		bool RecursiveExtractModelMesh(Struct::StaticModelData& a_staticModelData, FbxNode* a_fbxNode) const;

		bool ExtractModelMesh(Struct::ModelMesh& a_modelMesh, FbxMesh* a_fbxMesh) const;

		TypeAlias::Math::Vector3 FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex)													  const;
		TypeAlias::Math::Vector3 FetchVertexNormal  (const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex)						  const;
		TypeAlias::Math::Vector2 FetchVertexUV		(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex, const char* a_uvSetName) const;

		TypeAlias::Math::Vector3 ConvertFbxVector4ToVector3(const FbxVector4& a_fbxVector) const;
		TypeAlias::Math::Vector2 ConvertFbxVector2ToVector2(const FbxVector2& a_fbxVector) const;

		static constexpr int k_emptyMeshCount	   = 0;
		static constexpr int k_emptyUVSetNameCount = 0;
		static constexpr int k_firstUVSetNameIndex = 0;
		static constexpr int k_triangleVertexCount = 3;

		static constexpr int k_fbxVectorXIndex = 0;
		static constexpr int k_fbxVectorYIndex = 1;
		static constexpr int k_fbxVectorZIndex = 2;

		static constexpr bool k_isReplaceOriginalMesh = true;
	};
}