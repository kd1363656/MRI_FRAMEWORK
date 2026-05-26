#include "LightSystem.h"

void FWK::Graphics::LightSystem::ApplyDefaultLightDirection()
{
	m_directionalLight.m_direction = Constant::k_defaultDirectionalLightDirection;
	m_directionalLight.m_color     = Constant::k_defaultDirectionalLightColor;
	m_directionalLight.m_intensity = Constant::k_defaultDirectionalLightIntensity;

	// ライト方向はシェーダー側で内積計算に使うため、長さ1に正規化しておく
	// 正規化していないと、方向ベクトルの長さによって明るさまで変わってしまうため
	m_directionalLight.m_direction.Normalize();
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