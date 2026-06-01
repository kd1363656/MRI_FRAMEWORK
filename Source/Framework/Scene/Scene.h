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

		std::shared_ptr<Graphics::Camera> m_camera = std::make_shared<Graphics::Camera>();

		std::shared_ptr<Graphics::Texture>	   m_texture     = std::make_shared<Graphics::Texture>	  ();
		std::shared_ptr<Graphics::StaticModel> m_staticModel = std::make_shared<Graphics::StaticModel>();
	};
}