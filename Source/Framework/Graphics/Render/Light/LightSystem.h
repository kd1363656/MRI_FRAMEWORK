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

		static constexpr TypeAlias::Math::Vector3 k_defaultDirectionalLightDirection = { -0.3F,  0.0F,  0.3F };
		static constexpr TypeAlias::Math::Vector3 k_defaultDirectionalLightColor     = {  0.80F, 0.80F, 0.80F };
		static constexpr TypeAlias::Math::Vector3 k_ambientColor					 = {  1.0F,  1.0F,  1.0F };

		static constexpr float k_directionalLightIntensity = 1.0F;
		static constexpr float k_ambientLightIntensity	   = 0.25F;

		Struct::DirectionalLight m_directionalLight = {};
		Struct::AmbientLight	 m_ambientLight     = {};
	};
}