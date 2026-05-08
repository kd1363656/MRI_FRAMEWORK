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
		assert(false && "静的モデルファイルの読み込みに失敗しました。");
		return false;
	}

	Struct::ModelData l_modelData = {};

	if (!m_fbxModelLoader.LoadStaticModelFile(a_filePath, l_modelData))
	{
		assert(false && "FBXからModelDataの作成に失敗しました。");
		return false;
	}

	std::vector<Struct::ModelMeshletData> l_modelMeshletDataList(l_modelData.m_modelMeshList.size());

	for (std::size_t l_meshIndex = 0ULL; l_meshIndex < l_modelData.m_modelMeshList.size(); ++l_meshIndex)
	{
		auto& l_modelMesh        = l_modelData.m_modelMeshList[l_meshIndex];
		auto& l_modelMeshletData = l_modelMeshletDataList     [l_meshIndex];

		if (!m_modelMeshletBuilder.BuildModelMeshletData(l_modelMesh, l_modelMeshletData))
		{
			assert(false && "ModelMeshからModelMeshletDataの作成に失敗しました。");
			return false;
		}
	}

	auto l_assetFilePath = a_filePath;

	l_assetFilePath.replace_extension(Constant::k_lowerAssetExtension);

	Converter::ModelBinaryConverter l_modelBinaryConverter = {};

	if (!l_modelBinaryConverter.SaveStaticModelAsset(l_modelMeshletDataList, l_modelData, l_assetFilePath))
	{
		assert(false && "ModelDataとModelMeshletDataのModelAsset保存に失敗しました。");
		return false;
	}

	return true;
}