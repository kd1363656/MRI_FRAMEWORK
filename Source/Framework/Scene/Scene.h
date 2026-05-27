#pragma once

namespace FWK
{
	class Scene final
	{
	public:

		 Scene() = default;
		~Scene() = default;

		void PostLoadSetup();

		void RegisterDrawCommand() const;

		void Update();

	private:

		void RegisterDrawCommandTexture(const std::shared_ptr<Graphics::Texture>& a_texture) const;

		template <Concept::IsDerivedDrawCommandBaseConcept Type>
		void RegisterDrawStaticModelStandard(const std::shared_ptr<Graphics::StaticModel>& a_staticModel,  const std::shared_ptr<Graphics::Camera>& a_camera, const std::shared_ptr<Struct::StaticModelStandardDrawCommand>& a_staticModelStandardDrawCommand) const
		{
			if (!a_staticModel)			   { return; }
			if (!a_staticModel->IsValid()) { return; }
			if (!a_camera)				   { return; }

			const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
			const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	  ();
			const auto& l_renderGraph	  = l_renderer.GetREFRenderGraph		  ();

			const auto l_drawCommand = l_renderGraph.FindVALDrawCommand<Type>();

			if (!l_drawCommand) { return; }

			l_drawCommand->RegisterPassConstant(m_staticModelStandardPassConstant);
			l_drawCommand->RegisterDrawCommand (a_staticModelStandardDrawCommand);
		}
		
		std::shared_ptr<Graphics::Camera> m_camera = std::make_shared<Graphics::Camera>();

		std::shared_ptr<Graphics::Texture>	   m_texture     = std::make_shared<Graphics::Texture>	  ();
		std::shared_ptr<Graphics::StaticModel> m_staticModel = std::make_shared<Graphics::StaticModel>();

		std::shared_ptr<Struct::StaticModelStandardDrawCommand>  m_staticModelStandardLitDrawCommand   = std::make_shared<Struct::StaticModelStandardDrawCommand> ();
		std::shared_ptr<Struct::StaticModelStandardDrawCommand>  m_staticModelStandardUnLitDrawCommand = std::make_shared<Struct::StaticModelStandardDrawCommand> ();
		std::shared_ptr<Struct::StaticModelStandardPassConstant> m_staticModelStandardPassConstant     = std::make_shared<Struct::StaticModelStandardPassConstant>();
		std::shared_ptr<Struct::SpriteStandardDrawCommand>		 m_spriteStandardDrawCommand	       = std::make_shared<Struct::SpriteStandardDrawCommand>	  ();

	};
}