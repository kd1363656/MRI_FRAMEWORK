#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase) };

	// テクスチャ
	struct RootParameterCBSpritePassTag      final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag,   RootParameterTagBase) };
	struct RootParameterCBSpritePerObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePerObjectTag, RootParameterTagBase) };

	// カメラ
	struct RootParameterCBCameraPassTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBCameraPassTag, RootParameterTagBase) };

	// ライト
	struct RootParameterCBLightPassTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBLightPassTag, RootParameterTagBase) };

	// Model
	struct RootParameterCBModelPerObjectTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBModelPerObjectTag, RootParameterTagBase) };

	// DeferredLighting
	struct RootParameterCBDeferredLightingPassTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBDeferredLightingPassTag, RootParameterTagBase) };

	// PostEffect
	struct RootParameterCBPostEffectPassTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBPostEffectPassTag, RootParameterTagBase) };

	// FinalPresent
	struct RootParameterCBFinalPresentPassTag : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBFinalPresentPassTag, RootParameterTagBase) };
}