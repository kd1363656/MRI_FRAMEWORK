#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	m_texture.Load("Asset/Texture/Test.dds");
}

void FWK::Scene::RequestDraw() const
{
	auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance ();
	auto& l_renderer		= l_graphicsManager.GetMutableREFRenderer();
	
	// 描画用コマンドクラスを呼び出し必要な値を格納
	auto l_drawSpriteStandardDrawCommand = l_renderer.FetchVALDrawCommand<Graphics::DrawSpriteStandardCommand>();
	if (!l_drawSpriteStandardDrawCommand) { return; }

	Struct::SpriteDrawCommand l_spriteDrawCommand = {};

	l_spriteDrawCommand.m_textureID = m_texture.GetVALTextureID();

	// 描画申請
	l_drawSpriteStandardDrawCommand->RequestDraw(l_spriteDrawCommand);
}