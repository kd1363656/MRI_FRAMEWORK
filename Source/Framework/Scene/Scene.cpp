#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	// テクスチャ
	m_texture.Load("Asset/Texture/Test.dds");

	// モデル
	m_staticModel.Load("Asset/Model/Antike.fbx");

	m_camera = std::make_shared<Graphics::Camera>();
	
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

	Struct::SpriteStandardDrawCommand l_spriteStandardDrawCommand = {};

	l_spriteStandardDrawCommand.m_textureRecord = a_texture.GetREFTextureRecord();
	l_spriteStandardDrawCommand.m_color         = { 0.0F, 0.0F, 0.0F, 1.00F };
	l_spriteStandardDrawCommand.m_position      = { 0.0F, 0.0F };
	l_spriteStandardDrawCommand.m_sourceRECT    = { 0L, 0L, 256L, 256L };

	l_drawCommand->RequestDraw(l_spriteStandardDrawCommand);
}