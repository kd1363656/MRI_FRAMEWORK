#include "RenderGraphPassBase.h"

void FWK::Graphics::RenderGraphPassBase::ReadTexture(const D3D12_RESOURCE_STATES a_requiredState, const TypeAlias::TypeTag a_textureTag)
{
	AddTextureAccess(a_requiredState, a_textureTag, Utility::Tag::GetTag<Tag::RenderGraphReadAccessTag>());
}

void FWK::Graphics::RenderGraphPassBase::WriteTexture(const D3D12_RESOURCE_STATES a_requiredState, const TypeAlias::TypeTag a_textureTag)
{
	AddTextureAccess(a_requiredState, a_textureTag, Utility::Tag::GetTag<Tag::RenderGraphWriteAccessTag>());
}

void FWK::Graphics::RenderGraphPassBase::AddTextureAccess(const D3D12_RESOURCE_STATES a_requiredState, const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_accessTag)
{
	if (a_textureTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderGraphTextureAccessのTextureTagが無効です。");
		return;
	}

	if (a_accessTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderGraphTextureAccessのAccessTagが無効です。");
		return;
	}

	Struct::RenderGraphTextureAccess l_textureAccess = {};

	l_textureAccess.m_textureTag    = a_textureTag;
	l_textureAccess.m_accessTag     = a_accessTag;
	l_textureAccess.m_requiredState = a_requiredState;

	m_textureAccessList.emplace_back(l_textureAccess);
}