#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	m_texture.Load("Asset/Texture/Test.dds");

	// モデルロード
	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance ();
	const auto& l_resourceContext = l_graphicsManager.GetREFResourceContext();
	
	l_resourceContext.GetREFModelSystem().LoadStaticModel("Asset/Model/Antike.fbx");
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

	l_spriteDrawCommand.m_textureID  = a_texture.GetVALTextureID();
	l_spriteDrawCommand.m_color      = { 0.0F, 0.0F, 0.0F, 1.00F };
	l_spriteDrawCommand.m_position   = { 0.0F, 0.0F };
	l_spriteDrawCommand.m_sourceRECT = { 0L, 0L, 256L, 256L };

	l_drawCommand->RequestDraw(l_spriteDrawCommand);
}