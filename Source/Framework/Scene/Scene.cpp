#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	// テクスチャ
	if (m_texture)
	{
		m_texture->Load("Asset/Texture/Test.dds");
	}

	// モデル
	if (m_staticModel)
	{
		m_staticModel->Load("Asset/Model/Antike.fbx");
	}

	m_camera = std::make_shared<Graphics::Camera>();

	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance       ();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	         ();
	const auto& l_viewport        = l_renderer.GetREFRenderArea().GetREFViewport ();

	const auto l_aspectRatio = l_viewport.Width / l_viewport.Height;

	m_camera->SetupPerspective(TypeAlias::Math::Matrix::CreateTranslation(0.0F, 1.0F, -1.15F), l_aspectRatio);
}

void FWK::Scene::RegisterDrawCommand() const
{

}

void FWK::Scene::Update()
{
	static TypeAlias::Math::Vector3 l_cameraPos = { 0.0F, 1.0F, -1.15F };
	static float					l_rot		= 0.0F;

	if (GetAsyncKeyState(VK_SHIFT))
	{
		if (GetAsyncKeyState('W'))
		{
			l_cameraPos.y += 0.01F;
		}
		else if (GetAsyncKeyState('S'))
		{
			l_cameraPos.y -= 0.01F;
		}
	}
	else if (GetAsyncKeyState('W'))
	{
		l_cameraPos.z += 0.01F;
	}
	else if (GetAsyncKeyState('S'))
	{
		l_cameraPos.z -= 0.01F;
	}

	if (GetAsyncKeyState('A'))
	{
		l_cameraPos.x -= 0.01F;
	}
	else if (GetAsyncKeyState('D'))
	{
		l_cameraPos.x += 0.01F;
	}

	if (GetAsyncKeyState('A'))
	{
		l_cameraPos.x -= 0.01F;
	}
	else if (GetAsyncKeyState('D'))
	{
		l_cameraPos.x += 0.01F;
	}

	if (GetAsyncKeyState('Q'))
	{
		l_rot -= 1.0F;
	}
	else if (GetAsyncKeyState('E'))
	{
		l_rot += 1.0F;
	}

	if (GetAsyncKeyState('1'))
	{
		m_staticModel = nullptr;
	}

	if (GetAsyncKeyState('2'))
	{
		m_texture = nullptr;
	}

	m_camera->SetCameraMatrix(TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(l_rot)) * TypeAlias::Math::Matrix::CreateTranslation(l_cameraPos));
}

void FWK::Scene::RequestDrawTexture(const std::shared_ptr<Graphics::Texture>& a_texture) const
{
	if (!a_texture) { return; }

	const auto&	l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer      ();

	const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Graphics::DrawSpriteStandardCommand>();

	if (!l_drawCommand) { return; }

	m_spriteStandardDrawCommand->m_textureRecord = a_texture->GetREFTextureRecord();
	m_spriteStandardDrawCommand->m_color         = { 0.0F, 0.0F, 0.0F, 1.00F };
	m_spriteStandardDrawCommand->m_position      = { 0.0F, 0.0F };
	m_spriteStandardDrawCommand->m_sourceRECT    = { 0L, 0L, 256L, 256L };

	l_drawCommand->RequestDraw(m_spriteStandardDrawCommand);
}