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

		static constexpr std::string_view k_defaultSceneName		 = "ModelScene";
		static constexpr std::string_view k_defaultModelImporterName = "ModelScene";

		static constexpr int k_triangleVertexCount = 3;

		FbxManager* m_fbxManager = nullptr;
	};
}