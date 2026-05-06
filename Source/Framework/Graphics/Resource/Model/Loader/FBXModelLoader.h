#pragma once

namespace FWK::Graphics
{
	class FBXModelLoader
	{
	public:

		 FBXModelLoader() = default;
		~FBXModelLoader() = default;

		bool LoadModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData);

	private:

		bool ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::ModelData& a_modelData);
		
		bool ExtractMesh(FbxMesh* a_fbxMesh, Struct::ModelMesh& a_modelMesh) const;

		TypeAlias::Math::Vector3 FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const;

		TypeAlias::Math::Vector3 FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const;

		TypeAlias::Math::Vector2 FetchVertexUV(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const;
	};
}