#pragma once

namespace FWK
{
	class Scene final
	{
	public:

		 Scene() = default;
		~Scene() = default;

		void PostLoadSetup();

		void Update();

	private:

		std::shared_ptr<Graphics::Camera> m_camera = std::make_shared<Graphics::Camera>();

		std::shared_ptr<Graphics::Texture>	   m_texture     = std::make_shared<Graphics::Texture>	  ();
		std::shared_ptr<Graphics::StaticModel> m_staticModel = std::make_shared<Graphics::StaticModel>();

		std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData> m_staticModelDrawRequestData = std::make_shared<Struct::StaticModelStandardPerObjectDrawRequestData>();
	};
}