#include "LightSystem.h"

void FWK::Graphics::LightSystem::ApplyDefaultLightDirection()
{
	m_directionalLight.m_direction = Constant::k_defaultDirectionalLightDirection;
	m_directionalLight.m_color     = Constant::k_defaultDirectionalLightColor;
	m_directionalLight.m_intensity = Constant::k_defaultDirectionalLightIntensity;
}
void FWK::Graphics::LightSystem::ApplyDefaultAmbientLight()
{
	m_ambientLight.m_color     = Constant::k_defaultAmbientLightColor;
	m_ambientLight.m_intensity = Constant::k_defaultAmbientLightIntensity;
}

FWK::Struct::CBLight FWK::Graphics::LightSystem::CreateCBLight() const
{
	Struct::CBLight l_cbLight = {};

	l_cbLight.m_directionalLight = m_directionalLight;
	l_cbLight.m_ambientLight     = m_ambientLight;

	return l_cbLight;
}