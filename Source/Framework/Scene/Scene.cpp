#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	// テクスチャ
	m_texture.Load("Asset/Texture/Test.dds");

	// モデル
	auto& l_graphicsManager   = Graphics::GraphicsManager::GetInstance		  ();
	auto& l_staticModelSystem = l_graphicsManager.GetMutableREFResourceContext().GetMutableREFStaticModelSystem();

	if (!m_staticModelRecord)
	{
		m_staticModelRecord = std::make_shared<Struct::StaticModelRecord>();
	}
	
	const std::filesystem::path l_staticModelFilePath = "Asset/Model/Antike.fbx";

	const auto l_loadStartTime = std::chrono::high_resolution_clock::now();

	if (!l_staticModelSystem.LoadStaticModel(m_staticModelRecord, l_staticModelFilePath))
	{
		assert(false && "StaticModelの読み込みに失敗しました。");
		return;
	}

	const auto l_loadEndTime = std::chrono::high_resolution_clock::now();

	std::size_t l_vertexCount = 0ULL;
	std::size_t l_indexCount  = 0ULL;

	for (const auto& l_modelMesh : m_staticModelRecord->m_modelData.m_modelMeshList)
	{
		l_vertexCount += l_modelMesh.m_modelVertexList.size();
		l_indexCount  += l_modelMesh.m_indexList.size();
	}

	const auto l_loadTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(l_loadEndTime - l_loadStartTime).count();

	std::string l_debugLog = {};

	l_debugLog += std::format("StaticModelSystem Load Test\n");
	l_debugLog += std::format("ModelPath   : {}\n",    l_staticModelFilePath.string());
	l_debugLog += std::format("MeshCount   : {}\n",    m_staticModelRecord->m_modelData.m_modelMeshList.size());
	l_debugLog += std::format("VertexCount : {}\n",    l_vertexCount);
	l_debugLog += std::format("IndexCount  : {}\n",    l_indexCount);
	l_debugLog += std::format("Load Time   : {} ms\n", l_loadTimeMS);

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
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer      ();

	const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Graphics::DrawSpriteStandardCommand>();

	if (!l_drawCommand) { return; }

	Struct::SpriteDrawCommand l_spriteDrawCommand = {};

	l_spriteDrawCommand.m_textureRecord = a_texture.GetREFTextureRecord();
	l_spriteDrawCommand.m_color         = { 0.0F, 0.0F, 0.0F, 1.00F };
	l_spriteDrawCommand.m_position      = { 0.0F, 0.0F };
	l_spriteDrawCommand.m_sourceRECT    = { 0L, 0L, 256L, 256L };

	l_drawCommand->RequestDraw(l_spriteDrawCommand);
}