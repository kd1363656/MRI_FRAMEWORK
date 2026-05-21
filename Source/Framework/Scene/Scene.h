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

		void RequestDrawStaticModelUnLitStandard(const Graphics::StaticModel& a_staticModel, const std::shared_ptr<Graphics::Camera>& a_camera) const;
		void RequestDrawStaticModelLitStandard  (const Graphics::StaticModel& a_staticModel, const std::shared_ptr<Graphics::Camera>& a_camera) const;

		std::shared_ptr<Graphics::Camera> m_camera = nullptr;

		Graphics::Texture	  m_texture     = {};
		Graphics::StaticModel m_staticModel = {};
	};
}