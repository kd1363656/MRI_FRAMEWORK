#pragma once

namespace FWK::Tag
{
	struct SceneTextureTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(SceneTextureTagBase, TagBase) };

	// ForwardRenderingやDeferredLighting後の最終シーンカラー
	struct SceneColorTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SceneColorTextureTag, SceneTextureTagBase) };

	// モデルのベースカラーを保持するGBuffer
	struct GBufferAlbedoTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(GBufferAlbedoTextureTag, SceneTextureTagBase) };

	// モデルの法線を保持するGBuffer
	struct GBufferNormalTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(GBufferNormalTextureTag, SceneTextureTagBase) };

	// モデルのワールド座標を保持するGBuffer
	struct GBufferWorldPositionTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO(GBufferWorldPositionTextureTag, SceneTextureTagBase) };

	// ポストエフェクト後の色
	struct PostEffectColorTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(PostEffectColorTextureTag, SceneTextureTagBase) };

	// シーンのデプスステンシル
	struct SceneDepthStencilTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SceneDepthStencilTextureTag, SceneTextureTagBase) };
}