#pragma once

namespace FWK
{
	class Scene final
	{
	public:

		 Scene() = default;
		~Scene() = default;

		void PostLoadSetup();

	private:

		Graphics::Texture m_texture = {};
	};
}