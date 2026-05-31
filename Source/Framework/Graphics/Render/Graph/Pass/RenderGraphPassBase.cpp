#include "RenderGraphPassBase.h"

void FWK::Graphics::RenderGraphPassBase::ReadTexture(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_usageTag)
{
	AddTextureAccess(a_textureTag, Utility::Tag::GetVALTag<Tag::RenderGraphReadAccessTag>(), a_usageTag);
}

void FWK::Graphics::RenderGraphPassBase::WriteTexture(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_usageTag)
{
	AddTextureAccess(a_textureTag, Utility::Tag::GetVALTag<Tag::RenderGraphWriteAccessTag>(), a_usageTag);
}

void FWK::Graphics::RenderGraphPassBase::AddTextureAccess(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_accessTag, const TypeAlias::TypeTag a_usageTag)
{
	FWK_ASSERT_RETURN_IF(a_textureTag == Constant::k_invalidTypeTag, "RenderGraphTextureAccessのTextureTagが無効です。")
	FWK_ASSERT_RETURN_IF(a_accessTag == Constant::k_invalidTypeTag,  "RenderGraphTextureAccessのAccessTagが無効です。")
	FWK_ASSERT_RETURN_IF(a_usageTag == Constant::k_invalidTypeTag,   "RenderGraphTextureAccessのUsageTagが無効です。")

	Struct::RenderGraphTextureAccess l_textureAccess = {};

	l_textureAccess.m_textureTag = a_textureTag;
	l_textureAccess.m_accessTag  = a_accessTag;
	l_textureAccess.m_usageTag   = a_usageTag;

	m_textureAccessList.emplace_back(l_textureAccess);
}