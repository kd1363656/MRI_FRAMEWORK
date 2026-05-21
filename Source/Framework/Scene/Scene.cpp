#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	// テクスチャ
	m_texture.Load("Asset/Texture/Test.dds");

	// モデル
	m_staticModel.Load("Asset/Model/Antike.fbx");

	m_camera = std::make_shared<Graphics::Camera>();

	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance       ();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	         ();
	const auto& l_viewport        = l_renderer.GetREFRenderArea().GetREFViewport ();

	const auto l_aspectRatio = l_viewport.Width / l_viewport.Height;

	m_camera->SetupPerspective(TypeAlias::Math::Matrix::CreateTranslation(0.0F, 0.0F, -0.05F), l_aspectRatio);
}

void FWK::Scene::RequestDraw() const
{
	//RequestDrawTexture(m_texture);

	// UnLitモデルの描画
	RequestDrawStaticModelStandard<Graphics::DrawStaticModelUnLitStandardCommand>(m_staticModel, m_camera);
	
	// Litモデルの描画
	RequestDrawStaticModelStandard<Graphics::DrawStaticModelLitStandardCommand>(m_staticModel, m_camera);
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