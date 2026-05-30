#pragma once

namespace FWK::Tag
{
	struct RootSignatureTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootSignatureTagBase, TagBase) };

	// テクスチャ
	struct SpriteStandardRootSignatureTag final : public RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardRootSignatureTag, RootSignatureTagBase) };

	// モデル
	struct ModelUnLitStandardRootSignatureTag final : public RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelUnLitStandardRootSignatureTag, RootSignatureTagBase) };
	struct ModelLitStandardRootSignatureTag   final : public RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelLitStandardRootSignatureTag,   RootSignatureTagBase) };

	// ポストエフェクト
	struct PostEffectRootSignatureTag final : public RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(PostEffectRootSignatureTag, RootSignatureTagBase) };

	// 最終出力
	struct FinalPresentRootSignatureTag final : public RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(FinalPresentRootSignatureTag, RootSignatureTagBase) };
}