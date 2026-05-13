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

		Graphics::Texture m_texture = {};

		std::shared_ptr<Struct::StaticModelRecord> m_staticModelRecord = nullptr;
	};
}