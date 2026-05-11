#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	m_texture.Load("Asset/Texture/Test.dds");

	Graphics::StaticModelFBXLoader l_staticModelFBXLoader = {};
	Struct::ModelData			   l_modelData			  = {};

	// テスト
	// FBXファイルを読み込んで、ModelDataに変換できるか確認する
	// ここではまだ描画せず、頂点数、インデックス数が取れるかだけを見る
	if (!l_staticModelFBXLoader.LoadStaticModelFile("Asset/Model/Antike.fbx", l_modelData))
	{
		assert(false && "StaticModelFBXLoaderによるFBX読み込み確認に失敗しました。");
		return;
	}

	std::size_t l_totalVertexCount = 0ULL;
	std::size_t l_totalIndexCount  = 0ULL;

	// ModelDataには複数Meshが入る可能性があるため、全Meshの頂点数とIndex数を合計する
	for (const auto& l_modelMesh : l_modelData.m_modelMeshList)
	{
		l_totalVertexCount += l_modelMesh.m_modelVertexList.size();
		l_totalIndexCount += l_modelMesh.m_indexList.size();
	}

	std::string l_debugLog = {};
	l_debugLog += "StaticModel FBX Load Test\n";
	l_debugLog += "MeshCount   : " + std::to_string(l_modelData.m_modelMeshList.size()) + "\n";
	l_debugLog += "VertexCount : " + std::to_string(l_totalVertexCount) + "\n";
	l_debugLog += "IndexCount  : " + std::to_string(l_totalIndexCount) + "\n";

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