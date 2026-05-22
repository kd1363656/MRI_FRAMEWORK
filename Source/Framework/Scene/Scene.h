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

		void RequestDrawTexture(const Graphics::Texture& a_texture) const;

		template <Concept::IsDerivedDrawCommandBaseConcept Type>
		void RequestDrawStaticModelStandard(const Graphics::StaticModel& a_staticModel, const std::shared_ptr<Graphics::Camera>& a_camera) const
		{
			if (!a_staticModel.IsValid()) { return; }
			if (!a_camera)				  { return; }

			const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
			const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	  ();

			const auto& l_drawCommand = l_renderer.FindVALDrawCommand<Type>();

			if (!l_drawCommand) { return; }

			Struct::StaticModelStandardPassConstant l_staticModelUnLitStandardPassConstant = {};
			l_staticModelUnLitStandardPassConstant.m_camera = m_camera;

			Struct::StaticModelStandardDrawCommand  l_staticModelUnLitStandardDrawCommand  = {};

			static float l_rot = 0.0F;

			l_rot += 1.0F;

			if (l_rot >= 360.0F)
			{
				l_rot = l_rot - 360.0F;
			}

			l_staticModelUnLitStandardDrawCommand.m_staticModelRecord = a_staticModel.GetREFStaticModelRecord();
			l_staticModelUnLitStandardDrawCommand.m_worldMatrix		  = TypeAlias::Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(l_rot));
			//l_staticModelUnLitStandardDrawCommand.m_worldMatrix		  = TypeAlias::Math::Matrix::Identity;

			l_drawCommand->SetPassConstant(l_staticModelUnLitStandardPassConstant);
			l_drawCommand->RequestDraw    (l_staticModelUnLitStandardDrawCommand);
		}
		
		std::shared_ptr<Graphics::Camera> m_camera = nullptr;

		Graphics::Texture	  m_texture     = {};
		Graphics::StaticModel m_staticModel = {};
	};
}