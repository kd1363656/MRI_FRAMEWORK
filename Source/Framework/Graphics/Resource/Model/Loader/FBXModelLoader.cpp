#include "FBXModelLoader.h"

bool FWK::Graphics::FBXModelLoader::LoadModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData)
{
	if (a_filePath.empty())
	{
		assert(false && "読み込み対象のFBXモデルファイルパスが空のため、FBX読み込みに失敗しました。");
		return false;
	}

	if (!std::filesystem::exists(a_filePath))
	{
		assert(false && "読み込み対象のFBXモデルファイルが存在しないため、FBX読み込みに失敗しました。");
		return false;
	}

	if (!std::filesystem::is_regular_file(a_filePath))
	{
		assert(false && "読み込み対象のFBXモデルファイルパスが通常ファイルではないため、FBX読み込みに失敗しました。");
		return false;
	}

	if (a_filePath.extension() != Constant::k_lowerFBXExtension)
	{
		assert(false && "読み込み対象のモデルファイル拡張子が.fbxではないため、FBX読み込みに失敗しました。");
		return false;
	}

	// 念のため初期化
	a_modelData = {};

	return false;
}

bool FWK::Graphics::FBXModelLoader::ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::ModelData& a_modelData)
{
	return false;
}

bool FWK::Graphics::FBXModelLoader::ExtractMesh(FbxMesh* a_fbxMesh, Struct::ModelMesh& a_modelMesh) const
{
	return false;
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoader::FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const
{
	return TypeAlias::Math::Vector3();
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoader::FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const
{
	return TypeAlias::Math::Vector3();
}

FWK::TypeAlias::Math::Vector2 FWK::Graphics::FBXModelLoader::FetchVertexUV(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const
{
	return TypeAlias::Math::Vector2();
}