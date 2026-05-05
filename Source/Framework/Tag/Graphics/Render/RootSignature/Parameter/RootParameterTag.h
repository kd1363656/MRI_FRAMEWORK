#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase); };

	// テクスチャ関係
	struct RootParameterSpriteBaseColorTextureTag final : RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterSpriteBaseColorTextureTag, RootParameterTagBase); };
	struct RootParameterCBSpriteDrawTag			  final : RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpriteDrawTag,			  RootParameterTagBase); };
	struct RootParameterCBSpritePassTag			  final : RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag,			  RootParameterTagBase); };
}