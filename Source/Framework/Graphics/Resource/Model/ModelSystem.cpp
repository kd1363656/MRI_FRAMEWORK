#include "ModelSystem.h"

bool FWK::Graphics::ModelSystem::Create()
{
	if (!m_fbxModelLoader.Create())
	{
		assert(false && "FBXModelLoaderの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::ModelSystem::LoadStaticModel(const std::filesystem::path& a_filePath) const
{
	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerFBXExtension))
	{
		assert(false && "静的モデルファイルの読み込みに失敗したため、ModelMeshletDataListの作成に失敗しました。");
		return false;
	}

	if (Struct::StaticModelData l_staticModelData = {};
		!m_fbxModelLoader.LoadStaticModelFile(a_filePath, l_staticModelData))
	{
		assert(false && "FBXからStaticModelDataの作成に失敗しました。");
		return false;
	}

	return true;
}