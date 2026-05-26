#pragma once

namespace FWK
{
	class Scene final
	{
	public:

		 Scene() = default;
		~Scene() = default;

		void PostLoadSetup();

		void RequestDraw() const;

		void Update();

	private:

		void RequestDrawTexture(const std::shared_ptr<Graphics::Texture>& a_texture) const;

		template <Concept::IsDerivedDrawCommandBaseConcept Type>
		void RequestDrawStaticModelStandard(const std::shared_ptr<Graphics::StaticModel>& a_staticModel, const std::shared_ptr<Graphics::Camera>& a_camera) const
		{
			if (!a_staticModel)			   { return; }
			if (!a_staticModel->IsValid()) { return; }
			if (!a_camera)				   { return; }

			const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
			const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	  ();

			const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Type>();

			if (!l_drawCommand) { return; }

			Struct::StaticModelStandardPassConstant l_staticModelUnLitStandardPassConstant = {};
			l_staticModelUnLitStandardPassConstant.m_camera = m_camera;

			m_staticModelStandardDrawCommand->m_staticModelRecord = a_staticModel->GetREFStaticModelRecord();
			m_staticModelStandardDrawCommand->m_worldMatrix		  = TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0F));
			
			l_drawCommand->SetPassConstant(l_staticModelUnLitStandardPassConstant);
			l_drawCommand->RequestDraw    (m_staticModelStandardDrawCommand);
		}
		
		std::shared_ptr<Graphics::Camera> m_camera = nullptr;

		std::shared_ptr<Graphics::Texture>	   m_texture     = std::make_shared<Graphics::Texture>	  ();
		std::shared_ptr<Graphics::StaticModel> m_staticModel = std::make_shared<Graphics::StaticModel>();

		std::shared_ptr<Struct::StaticModelStandardDrawCommand>  m_staticModelStandardDrawCommand  = std::make_shared<Struct::StaticModelStandardDrawCommand> ();
		std::shared_ptr<Struct::StaticModelStandardPassConstant> m_staticModelStandardPassConstant = std::make_shared<Struct::StaticModelStandardPassConstant>();
		std::shared_ptr<Struct::SpriteStandardDrawCommand>		 m_spriteStandardDrawCommand	   = std::make_shared<Struct::SpriteStandardDrawCommand>	  ();

	};
}