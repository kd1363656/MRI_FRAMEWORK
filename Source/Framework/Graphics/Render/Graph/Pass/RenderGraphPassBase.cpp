#include "RenderGraphPassBase.h"

void FWK::Graphics::RenderGraphPassBase::ReadTexture(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_usageTag)
{
	AddTextureAccess(a_textureTag, Utility::Tag::GetTag<Tag::RenderGraphReadAccessTag>(), a_usageTag);
}

void FWK::Graphics::RenderGraphPassBase::WriteTexture(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_usageTag)
{
	AddTextureAccess(a_textureTag, Utility::Tag::GetTag<Tag::RenderGraphWriteAccessTag>(), a_usageTag);
}

void FWK::Graphics::RenderGraphPassBase::AddTextureAccess(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_accessTag, const TypeAlias::TypeTag a_usageTag)
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

	if (a_usageTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderGraphTextureAccessのUsageTagが無効です。");
		return;
	}

	Struct::RenderGraphTextureAccess l_textureAccess = {};

	l_textureAccess.m_textureTag = a_textureTag;
	l_textureAccess.m_accessTag  = a_accessTag;
	l_textureAccess.m_usageTag   = a_usageTag;

	m_textureAccessList.emplace_back(l_textureAccess);
}