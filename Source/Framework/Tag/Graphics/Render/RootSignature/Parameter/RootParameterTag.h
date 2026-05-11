#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase); };

	// テクスチャ関係
	struct RootParameterSpriteBaseColorTextureTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterSpriteBaseColorTextureTag, RootParameterTagBase); };
	struct RootParameterCBSpriteDrawTag			  final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpriteDrawTag,			  RootParameterTagBase); };
	struct RootParameterCBSpritePassTag			  final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag,			  RootParameterTagBase); };
}