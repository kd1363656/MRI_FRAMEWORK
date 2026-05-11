#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	m_texture.Load("Asset/Texture/Test.dds");

	Graphics::StaticModelFBXLoader	   l_staticModelFBXLoader     = {};
	Graphics::StaticModelMeshOptimizer l_staticModelMeshOptimizer = {};
	Struct::ModelData				   l_modelData			      = {};

	// テスト
	// FBXファイルを読み込んで、ModelDataに変換できるか確認する
	// ここではまだ描画せず、頂点数、インデックス数が取れるかだけを見る
	if (!l_staticModelFBXLoader.LoadStaticModelFile("Asset/Model/Antike.fbx", l_modelData))
	{
		assert(false && "StaticModelFBXLoaderによるFBX読み込み確認に失敗しました。");
		return;
	}

	std::size_t l_beforeOptimizeVertexCount = 0ULL;
	std::size_t l_beforeOptimizeIndexCount  = 0ULL;
	
	// ModelDataには複数Meshが入る可能性があるため、全Meshの頂点数とIndex数を合計する
	for (const auto& l_modelMesh : l_modelData.m_modelMeshList)
	{
		l_beforeOptimizeVertexCount += l_modelMesh.m_modelVertexList.size();
		l_beforeOptimizeIndexCount  += l_modelMesh.m_indexList.size();
	}
	
	if (!l_staticModelMeshOptimizer.OptimizeModelData(l_modelData))
	{
		assert(false && "StaticModelMeshOptimizerによるModelDataの最適化に失敗しました。");
		return;
	}
	
	std::size_t l_afterOptimizeVertexCount = 0ULL;
	std::size_t l_afterOptimizeIndexCount  = 0ULL;
	
	// 最適化後の頂点数とIndex数を確認する
	for (const auto& l_modelMesh : l_modelData.m_modelMeshList)
	{
		l_afterOptimizeVertexCount += l_modelMesh.m_modelVertexList.size();
		l_afterOptimizeIndexCount  += l_modelMesh.m_indexList.size();
	}
	
	std::string l_debugLog = {};
	l_debugLog += "StaticModel Mesh Optimize Test\n";
	l_debugLog += "MeshCount         : " + std::to_string(l_modelData.m_modelMeshList.size()) + "\n";
	l_debugLog += "Before VertexCount: " + std::to_string(l_beforeOptimizeVertexCount) + "\n";
	l_debugLog += "Before IndexCount : " + std::to_string(l_beforeOptimizeIndexCount) + "\n";
	l_debugLog += "After VertexCount : " + std::to_string(l_afterOptimizeVertexCount) + "\n";
	l_debugLog += "After IndexCount  : " + std::to_string(l_afterOptimizeIndexCount) + "\n";
	
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

	const auto& l_drawCommand = l_renderer.FetchVALDrawCommand<Graphics::DrawSpriteStandardCommand>();

	if (!l_drawCommand) { return; }

	Struct::SpriteDrawCommand l_spriteDrawCommand = {};

	l_spriteDrawCommand.m_storageID  = a_texture.GetVALStorageID();
	l_spriteDrawCommand.m_color      = { 0.0F, 0.0F, 0.0F, 1.00F };
	l_spriteDrawCommand.m_position   = { 0.0F, 0.0F };
	l_spriteDrawCommand.m_sourceRECT = { 0L, 0L, 256L, 256L };

	l_drawCommand->RequestDraw(l_spriteDrawCommand);
}