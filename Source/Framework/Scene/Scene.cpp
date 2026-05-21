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

	m_camera->SetupPerspective(TypeAlias::Math::Matrix::CreateTranslation(0.0F, 0.12F, -0.05F), l_aspectRatio);
}

void FWK::Scene::RequestDraw() const
{
	//RequestDrawTexture(m_texture);

	// UnLitモデルの描画
	//RequestDrawStaticModelUnLitStandard(m_staticModel, m_camera);
	
	// Litモデルの描画
	RequestDrawStaticModelLitStandard(m_staticModel, m_camera);
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

void FWK::Scene::RequestDrawStaticModelUnLitStandard(const Graphics::StaticModel& a_staticModel, const std::shared_ptr<Graphics::Camera>& a_camera) const
{
	if (!a_staticModel.IsValid()) { return; }
	if (!a_camera)				  { return; }

	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	  ();

	const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Graphics::DrawStaticModelUnLitStandardCommand>();

	if (!l_drawCommand) { return; }

	Struct::StaticModelUnLitStandardPassConstant l_staticModelUnLitStandardPassConstant = {};
	l_staticModelUnLitStandardPassConstant.m_camera = m_camera;

	Struct::StaticModelUnLitStandardDrawCommand  l_staticModelUnLitStandardDrawCommand  = {};

	l_staticModelUnLitStandardDrawCommand.m_staticModelRecord = a_staticModel.GetREFStaticModelRecord();
	l_staticModelUnLitStandardDrawCommand.m_worldMatrix		  = TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0F));

	l_drawCommand->SetPassConstant(l_staticModelUnLitStandardPassConstant);
	l_drawCommand->RequestDraw(l_staticModelUnLitStandardDrawCommand);
}

void FWK::Scene::RequestDrawStaticModelLitStandard(const Graphics::StaticModel& a_staticModel, const std::shared_ptr<Graphics::Camera>& a_camera) const
{
	if (!a_staticModel.IsValid()) { return; }
	if (!a_camera)				  { return; }

	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	  ();

	const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Graphics::DrawStaticModelLitStandardCommand>();

	if (!l_drawCommand) { return; }

	Struct::StaticModelLitStandardPassConstant l_staticModelLitStandardPassConstant = {};
	l_staticModelLitStandardPassConstant.m_camera = m_camera;

	Struct::StaticModelLitStandardDrawCommand  l_staticModelLitStandardDrawCommand  = {};

	l_staticModelLitStandardDrawCommand.m_staticModelRecord = a_staticModel.GetREFStaticModelRecord();
	l_staticModelLitStandardDrawCommand.m_worldMatrix		= TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0F));

	l_drawCommand->SetPassConstant(l_staticModelLitStandardPassConstant);
	l_drawCommand->RequestDraw    (l_staticModelLitStandardDrawCommand);
}