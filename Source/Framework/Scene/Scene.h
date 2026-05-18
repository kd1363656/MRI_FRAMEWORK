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

		std::shared_ptr<Graphics::Camera> m_camera = nullptr;

		Graphics::Texture	  m_texture     = {};
		Graphics::StaticModel m_staticModel = {};
	};
}