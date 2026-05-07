#pragma once

namespace FWK::Graphics
{
	class FBXModelLoader final
	{
	public:

		 FBXModelLoader();
		~FBXModelLoader();

		bool Create();

		bool LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::StaticModelData& a_staticModelData) const;

	private:

		void Destroy();

		bool ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::StaticModelData& a_staticModelData) const;
		
		bool ExtractMesh(FbxMesh* a_fbxMesh, Struct::StaticModelMesh& a_staticModelMesh) const;

		TypeAlias::Math::Vector3 FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const;

		TypeAlias::Math::Vector3 FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const;

		TypeAlias::Math::Vector2 FetchVertexUV(const FbxMesh* a_fbxMesh, 
											   const int      a_polygonIndex,
											   const int	  a_polygonVertexIndex,
											   const char*	  a_uvSetName) const;

		TypeAlias::Math::Vector3 ConvertFbxVector4ToVector3(const FbxVector4& a_fbxVector) const;

		static constexpr std::string_view k_defaultSceneName		 = "ModelScene";
		static constexpr std::string_view k_defaultModelImporterName = "ModelImporter";

		static constexpr float k_uvVFlipOffset = 1.0F;

		static constexpr int k_autoDetectFBXFileFormatID = -1;
		static constexpr int k_emptyPolygonCount		 =  0;
		static constexpr int k_emptyUVSetCount           =  0;
		static constexpr int k_minControlPointIndex      =  0;
		static constexpr int k_firstUVSetIndex           =  0;

		static constexpr int k_fbxVectorXIndex = 0;
		static constexpr int k_fbxVectorYIndex = 1;
		static constexpr int k_fbxVectorZIndex = 2;

		static constexpr int k_fbxUVUIndex = 0;
		static constexpr int k_fbxUVVIndex = 1;

		FbxManager* m_fbxManager = nullptr;
	};
}