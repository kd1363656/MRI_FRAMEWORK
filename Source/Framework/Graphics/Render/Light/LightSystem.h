#pragma once

namespace FWK::Graphics
{
	class LightSystem final
	{
	public:

		 LightSystem() = default;
		~LightSystem() = default;

		void ApplyDefaultLightDirection();
		void ApplyDefaultAmbientLight  ();

		Struct::CBLight CreateCBLight() const;

	private:

		Struct::DirectionalLight m_directionalLight = {};
		Struct::AmbientLight	 m_ambientLight     = {};
	};
}