#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase); };

	// テクスチャ関係
	struct RootParameterCBSpritePassTag   final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag,   RootParameterTagBase); };
	struct RootParameterCBSpriteObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpriteObjectTag, RootParameterTagBase); };

	// カメラ関係
	struct RootParameterCBCameraTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBCameraTag, RootParameterTagBase); };

	// ライト関係
	struct RootParameterCBLightTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBLightTag, RootParameterTagBase); };

	// Model共通
	struct RootParameterCBModelObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBModelObjectTag, RootParameterTagBase); };

	// FinalPresent関係
	struct RootParameterCBFinalPresentTag : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBFinalPresentTag, RootParameterTagBase); };

}