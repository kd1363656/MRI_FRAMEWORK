#include "LightSystem.h"

void FWK::Graphics::LightSystem::ApplyDefaultLightDirection()
{
	m_directionalLight.m_direction = k_defaultDirectionalLightDirection;
	m_directionalLight.m_color     = k_defaultDirectionalLightColor;
	m_directionalLight.m_intensity = k_directionalLightIntensity;
}
void FWK::Graphics::LightSystem::ApplyDefaultAmbientLight()
{
	m_ambientLight.m_color     = k_ambientColor;
	m_ambientLight.m_intensity = k_ambientLightIntensity;
}

FWK::Struct::CBLight FWK::Graphics::LightSystem::CreateCBLight() const
{
	Struct::CBLight l_cbLight = {};

	l_cbLight.m_directionalLight = m_directionalLight;
	l_cbLight.m_ambientLight     = m_ambientLight;

	return l_cbLight;
}