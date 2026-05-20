#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase); };

	// テクスチャ関係
	struct RootParameterCBSpriteDrawTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpriteDrawTag, RootParameterTagBase); };
	struct RootParameterCBSpritePassTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag, RootParameterTagBase); };

	// カメラ関係
	struct RootParameterCBCameraTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBCameraTag, RootParameterTagBase); };

	// Model共通
	struct RootParameterCBModelObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBModelObjectTag, RootParameterTagBase); };
}