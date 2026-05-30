#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase) };

	// テクスチャ
	struct RootParameterCBSpritePassTag   final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag,   RootParameterTagBase) };
	struct RootParameterCBSpriteObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpriteObjectTag, RootParameterTagBase) };

	// カメラ
	struct RootParameterCBCameraTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBCameraTag, RootParameterTagBase) };

	// ライト
	struct RootParameterCBLightTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBLightTag, RootParameterTagBase) };

	// Model
	struct RootParameterCBModelObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBModelObjectTag, RootParameterTagBase) };

	// PostEffect
	struct RootParameterCBPostEffectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBPostEffectTag, RootParameterTagBase) };

	// FinalPresent
	struct RootParameterCBFinalPresentTag : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBFinalPresentTag, RootParameterTagBase) };
}