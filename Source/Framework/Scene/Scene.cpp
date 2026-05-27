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

	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance       ();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	         ();
	const auto& l_viewport        = l_renderer.GetREFRenderArea().GetREFViewport ();

	const auto l_aspectRatio = l_viewport.Width / l_viewport.Height;

	m_camera->SetupPerspective(TypeAlias::Math::Matrix::CreateTranslation(0.0F, 1.0F, -1.15F), l_aspectRatio);
}

void FWK::Scene::RegisterDrawCommand() const
{
	if (m_staticModel)
	{
		const auto& l_matrixA = TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0F)) * TypeAlias::Math::Matrix::CreateTranslation(TypeAlias::Math::Vector3( 1.0F, 0.0F, 0.0F));
		const auto& l_matrixB = TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0F)) * TypeAlias::Math::Matrix::CreateTranslation(TypeAlias::Math::Vector3(-1.0F, 0.0,  0.0F));

		m_staticModelStandardLitDrawCommand->m_staticModelRecord   = m_staticModel->GetREFStaticModelRecord();
		m_staticModelStandardUnLitDrawCommand->m_staticModelRecord = m_staticModel->GetREFStaticModelRecord();

		m_staticModelStandardLitDrawCommand->m_worldMatrix   = l_matrixA;
		m_staticModelStandardUnLitDrawCommand->m_worldMatrix = l_matrixB;

		m_staticModelStandardPassConstant->m_camera = m_camera;

		RegisterDrawStaticModelStandard<Graphics::DrawStaticModelLitStandardCommand>  (m_staticModel, m_camera, m_staticModelStandardLitDrawCommand);
		RegisterDrawStaticModelStandard<Graphics::DrawStaticModelUnLitStandardCommand>(m_staticModel, m_camera, m_staticModelStandardUnLitDrawCommand);
	}

	if (m_texture)
	{
		RegisterDrawCommandTexture(m_texture);
	}
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
		m_staticModel					      = nullptr;
		m_staticModelStandardUnLitDrawCommand = nullptr;
	}

	if (GetAsyncKeyState('2'))
	{
		m_staticModelStandardLitDrawCommand = nullptr;
	}

	if (GetAsyncKeyState('3'))
	{
		m_texture = nullptr;
	}

	m_camera->SetCameraMatrix(TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(l_rot)) * TypeAlias::Math::Matrix::CreateTranslation(l_cameraPos));
}

void FWK::Scene::RegisterDrawCommandTexture(const std::shared_ptr<Graphics::Texture>& a_texture) const
{
	if (!a_texture) { return; }

	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer      ();
	const auto& l_renderGraph	  = l_renderer.GetREFRenderGraph          ();

	const auto& l_drawCommand = l_renderGraph.FindVALDrawCommand<Graphics::DrawSpriteStandardCommand>();

	if (!l_drawCommand) { return; }

	m_spriteStandardDrawCommand->m_textureRecord = a_texture->GetREFTextureRecord();
	m_spriteStandardDrawCommand->m_color         = { 0.0F, 0.0F, 0.0F, 1.00F };
	m_spriteStandardDrawCommand->m_position      = { 0.0F, 0.0F };
	m_spriteStandardDrawCommand->m_sourceRECT    = { 0L, 0L, 256L, 256L };

	l_drawCommand->RegisterDrawCommand(m_spriteStandardDrawCommand);
}