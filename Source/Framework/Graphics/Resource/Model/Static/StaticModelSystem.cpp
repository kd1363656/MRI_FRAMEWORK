#include "StaticModelSystem.h"

bool FWK::Graphics::StaticModelSystem::LoadStaticModel(Struct::ModelData& a_modelData, const std::filesystem::path& a_fbxFilePath)
{
	if (a_fbxFilePath.empty())
	{
		assert(false && "StaticModelのFBXファイルパスが空です。");
		return false;
	}

	if (!std::filesystem::exists(a_fbxFilePath))
	{
		assert(false && "StaticModelのFBXファイルが存在しません。");
		return false;
	}

	const auto l_assetFilePath = Utility::File::CreateFilePathByReplaceExtension(a_fbxFilePath, Constant::k_lowerAssetExtension);

	if (CanUseStaticModelAsset(a_fbxFilePath, l_assetFilePath))
	{
		// バイナリーファイルを読み込めるなら読み込む
		if (LoadStaticModelAsset(a_modelData, l_assetFilePath)) { return true; }

		// .assetが存在していても、読み込みに失敗した場合は壊れている可能性があるためFBXから再生成する
		a_modelData.m_modelMeshList.clear();
	}
	
	return CreateStaticModelAssetFromFBX(a_modelData, a_fbxFilePath, l_assetFilePath);
}

bool FWK::Graphics::StaticModelSystem::CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const
{
	if (!std::filesystem::exists(a_assetFilePath)) { return false; }

	const auto& l_fbxLastWriteTime   = std::filesystem::last_write_time(a_fbxFilePath);
	const auto& l_assetLastWriteTime = std::filesystem::last_write_time(a_assetFilePath);

	// FBXのほうが新しい場合、assetは古いので使わない
	if (l_assetLastWriteTime < l_fbxLastWriteTime) { return false; }

	return true;
}

bool FWK::Graphics::StaticModelSystem::LoadStaticModelAsset(Struct::ModelData& a_modelData, const std::filesystem::path& a_assetFilePath)
{
	a_modelData.m_modelMeshList.clear();

	if (!m_staticModelBinaryConverter.LoadStaticModelAsset(a_modelData, a_assetFilePath)) { return false; }

	return true;
}

bool FWK::Graphics::StaticModelSystem::CreateStaticModelAssetFromFBX(Struct::ModelData& a_modelData, const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath)
{
	a_modelData.m_modelMeshList.clear();

	if (!m_staticModelFBXLoader.LoadStaticModelFile(a_fbxFilePath, a_modelData))
	{
		assert(false && "StaticModelFBXLoaderによるFBX読み込みに失敗しました。");
		return false;
	}

	if (!m_staticModelMeshOptimizer.OptimizeModelData(a_modelData))
	{
		assert(false && "StaticModelMeshOptimizerによるModelDataの最適化に失敗しました。");
		return false;
	}

	if (!m_staticModelBinaryConverter.SaveStaticModelAsset(a_modelData, a_assetFilePath))
	{
		assert(false && "StaticModelAssetの保存に失敗しました。");
		return false;
	}

	return true;
}