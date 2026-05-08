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

	Struct::StaticModelData l_staticModelData = {};

	if (!m_fbxModelLoader.LoadStaticModelFile(a_filePath, l_staticModelData))
	{
		assert(false && "FBXからStaticModelDataの作成に失敗しました。");
		return false;
	}

	std::vector<Struct::ModelMeshletData> l_modelMeshletDataList(l_staticModelData.m_staticModelMeshList.size());

	for (std::size_t l_meshIndex = 0ULL; l_meshIndex < l_staticModelData.m_staticModelMeshList.size(); ++l_meshIndex)
	{
		const auto& l_staticModelMesh = l_staticModelData.m_staticModelMeshList[l_meshIndex];

		auto& l_modelMeshletData = l_modelMeshletDataList[l_meshIndex];

		if (!m_modelMeshletBuilder.BuildStaticModelMeshletData(l_staticModelMesh, l_modelMeshletData))
		{
			assert(false && "StaticModelMeshからModelMeshletDataの作成に失敗しました。");
			return false;
		}
	}

	auto l_assetFilePath = a_filePath;

	l_assetFilePath.replace_extension(Constant::k_lowerAssetExtension);

	Converter::ModelBinaryConverter l_modelBinaryConverter = {};

	if (!l_modelBinaryConverter.SaveStaticModelAsset(l_modelMeshletDataList, l_staticModelData, l_assetFilePath))
	{
		assert(false && "StaticModelDataとModelMeshletDataのModelAsset保存に失敗しました。");
		return false;
	}

	return true;
}