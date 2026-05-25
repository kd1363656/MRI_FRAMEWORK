#include "RenderGraphPassBase.h"

void FWK::Graphics::RenderGraphPassBase::ReadTexture(const TypeAlias::TypeTag a_textureTag, const D3D12_RESOURCE_STATES a_requiredState)
{
	Struct::RenderGraphTextureAccess l_textureAccess = {};

	l_textureAccess.m_textureTag	= a_textureTag;
	l_textureAccess.m_requiredState = a_requiredState;

	m_readTextureAccessList.emplace_back(l_textureAccess);
}

void FWK::Graphics::RenderGraphPassBase::WriteTexture(const TypeAlias::TypeTag a_textureTag, const D3D12_RESOURCE_STATES a_requiredState)
{
	Struct::RenderGraphTextureAccess l_textureAccess = {};

	l_textureAccess.m_textureTag	= a_textureTag;
	l_textureAccess.m_requiredState = a_requiredState;

	m_writeTextureAccessList.emplace_back(l_textureAccess);
}