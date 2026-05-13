#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	m_texture.Load("Asset/Texture/Test.dds");

	const auto&								         l_modelData			      = std::make_shared<Struct::ModelData>();
	      Graphics::StaticModelFBXLoader	         l_staticModelFBXLoader       = {};
	      Graphics::StaticModelMeshOptimizer         l_staticModelMeshOptimizer   = {};
		  FWK::Converter::StaticModelBinaryConverter l_staticModelBinaryConverter = {};

	const std::filesystem::path& l_staticModelFilePath = "Asset/Model/Antike.fbx";

	const auto& l_staticModelAssetFilePath = FWK::Utility::File::CreateFilePathByReplaceExtension(l_staticModelFilePath, Constant::k_lowerAssetExtension);

	const auto& l_loadStartTime = std::chrono::high_resolution_clock::now();

	if (!l_staticModelFBXLoader.LoadStaticModelFile(l_modelData, l_staticModelFilePath))
	{
		assert(false && "StaticModelFBXLoaderによるFBX読み込み確認に失敗しました。");
		return;
	}

	const auto l_loadEndTime = std::chrono::high_resolution_clock::now();

	std::size_t l_beforOptimizeVertexCount = 0ULL;
	std::size_t l_beforOptimizeIndexCount  = 0ULL;

	// ModelDataには複数Meshが入る可能性があるため、全Meshの頂点数とIndex数を合計する
	for (const auto& l_modelMesh : l_modelData->m_modelMeshList)
	{
		l_beforOptimizeVertexCount += l_modelMesh.m_modelVertexList.size();
		l_beforOptimizeIndexCount  += l_modelMesh.m_indexList.size      ();
	}

	const auto l_optimizeStartTime = std::chrono::high_resolution_clock::now();

	if (!l_staticModelMeshOptimizer.OptimizeModelData(l_modelData))
	{
		assert(false && "StaticModelMeshOptimizerによるModelDataの最適化に失敗しました。");
		return;
	}

	const auto l_optimizeEndTime = std::chrono::high_resolution_clock::now();

	std::size_t l_afterOptimizeVertexCount = 0ULL;
	std::size_t l_afterOptimizeIndexCount  = 0ULL;

	// 最適化後の頂点数とIndex数を確認する
	for (const auto& l_modelMesh : l_modelData->m_modelMeshList)
	{
		l_afterOptimizeVertexCount += l_modelMesh.m_modelVertexList.size();
		l_afterOptimizeIndexCount  += l_modelMesh.m_indexList.size      ();
	}

	const auto l_assetSaveStartTime = std::chrono::high_resolution_clock::now();

	if (!l_staticModelBinaryConverter.SaveStaticModelAsset(l_modelData, l_staticModelAssetFilePath))
	{
		assert(false && "StaticModelAssetの保存に失敗しました。");
		return;
	}

	const auto l_assetSaveEndTime = std::chrono::high_resolution_clock::now();

	const auto& l_loadedModelData = std::make_shared<Struct::ModelData>();;

	const auto l_assetLoadStartTime = std::chrono::high_resolution_clock::now();

	if (!l_staticModelBinaryConverter.LoadStaticModelAsset(l_loadedModelData, l_staticModelAssetFilePath))
	{
		assert(false && "StaticModelAssetの読み込みに失敗しました。");
		return;
	}

	const auto l_assetLoadEndTime = std::chrono::high_resolution_clock::now();

	if (l_modelData->m_modelMeshList.size() != l_loadedModelData->m_modelMeshList.size())
	{
		assert(false && "保存前と読み込み後のMesh数が一致しません。");
		return;
	}

	std::size_t l_loadedVertexCount = 0ULL;
	std::size_t l_loadedIndexCount  = 0ULL;

	for (std::uint64_t l_meshIndex = 0ULL; l_meshIndex < l_modelData->m_modelMeshList.size(); ++l_meshIndex)
	{
		const auto& l_sourceModelMesh = l_modelData->m_modelMeshList      [l_meshIndex];
		const auto& l_loadedModelMesh = l_loadedModelData->m_modelMeshList[l_meshIndex];

		if (l_sourceModelMesh.m_modelVertexList.size() != l_loadedModelMesh.m_modelVertexList.size())
		{
			assert(false && "保存前と読み込み後のVertex数が一致しません。");
			return;
		}

		if (l_sourceModelMesh.m_indexList.size() != l_loadedModelMesh.m_indexList.size())
		{
			assert(false && "保存前と読み込み後のIndex数が一致しません。");
			return;
		}

		const auto& l_sourceModelMaterialAssetData = l_sourceModelMesh.m_modelMaterial.m_modelMaterialAssetData;
		const auto& l_loadedModelMaterialAssetData = l_loadedModelMesh.m_modelMaterial.m_modelMaterialAssetData;

		if (l_sourceModelMaterialAssetData.m_baseColorTextureFilePath != l_loadedModelMaterialAssetData.m_baseColorTextureFilePath)
		{
			assert(false && "BaseColorTexturePathが一致しません。");
			return;
		}

		if (l_sourceModelMaterialAssetData.m_normalTextureFilePath != l_loadedModelMaterialAssetData.m_normalTextureFilePath)
		{
			assert(false && "RoughnessTexturePathが一致しません。");
			return;
		}

		if (l_sourceModelMaterialAssetData.m_metallicTextureFilePath != l_loadedModelMaterialAssetData.m_metallicTextureFilePath)
		{
			assert(false && "MetallicTexturePathが一致しません。");
			return;
		}

		l_loadedVertexCount += l_loadedModelMesh.m_modelVertexList.size();
		l_loadedIndexCount  += l_loadedModelMesh.m_indexList.size      ();
	}

	const auto l_loadTimeMS      = std::chrono::duration_cast<std::chrono::milliseconds>(l_loadEndTime      - l_loadStartTime).count     ();
	const auto l_optimizeTimeMS  = std::chrono::duration_cast<std::chrono::milliseconds>(l_optimizeEndTime  - l_optimizeStartTime).count ();
	const auto l_assetSaveTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(l_assetSaveEndTime - l_assetSaveStartTime).count();
	const auto l_assetLoadTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(l_assetLoadEndTime - l_assetLoadStartTime).count();

	const auto l_fbxRouteTotalTimeMS    = l_loadTimeMS + l_optimizeTimeMS;
	const auto l_firstCreateTotalTimeMS = l_loadTimeMS + l_optimizeTimeMS + l_assetSaveTimeMS;

	std::string l_debugLog = {};

	l_debugLog += std::format("StaticModel Binary Benchmark Test\n");
	l_debugLog += std::format("AssetPath                  : {}\n" , l_staticModelAssetFilePath.string());
	l_debugLog += std::format("MeshCount                  : {}\n" , l_modelData->m_modelMeshList.size());
	l_debugLog += std::format("Before VertexCount         : {}\n" , l_beforOptimizeVertexCount);
	l_debugLog += std::format("Before IndexCount          : {}\n" , l_beforOptimizeIndexCount);
	l_debugLog += std::format("After  VertexCount         : {}\n" , l_afterOptimizeVertexCount);
	l_debugLog += std::format("After  IndexCount          : {}\n" , l_afterOptimizeIndexCount);
	l_debugLog += std::format("Loaded VertexCount         : {}\n" , l_loadedVertexCount);
	l_debugLog += std::format("Loaded IndexCount          : {}\n" , l_loadedIndexCount);
	l_debugLog += std::format("FBX Load Time			  : {}\n" , l_loadTimeMS);
	l_debugLog += std::format("Optimie Time			      : {}\n" , l_optimizeTimeMS);
	l_debugLog += std::format("Asset Save Time			  : {}\n" , l_assetSaveTimeMS);
	l_debugLog += std::format("Asset Load Time			  : {}\n" , l_assetLoadTimeMS);
	l_debugLog += std::format("FBX Route Total			  : {}\n" , l_fbxRouteTotalTimeMS);
	l_debugLog += std::format("First Create Total		  : {}\n" , l_firstCreateTotalTimeMS);
	l_debugLog += std::format("Binary Route Total		  : {}\n" , l_assetLoadTimeMS);
	
	OutputDebugStringA(l_debugLog.c_str());
}

void FWK::Scene::RequestDraw() const
{
	RequestDrawTexture(m_texture);
}

void FWK::Scene::Update()
{

}

void FWK::Scene::RequestDrawTexture(const Graphics::Texture& a_texture) const
{
	const auto&	l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer();

	const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Graphics::DrawSpriteStandardCommand>();

	if (!l_drawCommand) { return; }

	Struct::SpriteDrawCommand l_spriteDrawCommand = {};

	l_spriteDrawCommand.m_textureRecord = a_texture.GetREFTextureRecord();
	l_spriteDrawCommand.m_color         = { 0.0F, 0.0F, 0.0F, 1.00F };
	l_spriteDrawCommand.m_position      = { 0.0F, 0.0F };
	l_spriteDrawCommand.m_sourceRECT    = { 0L, 0L, 256L, 256L };

	l_drawCommand->RequestDraw(l_spriteDrawCommand);
}