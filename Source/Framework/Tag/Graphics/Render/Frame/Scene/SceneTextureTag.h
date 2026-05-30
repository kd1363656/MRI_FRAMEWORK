#pragma once

namespace FWK::Tag
{
	struct SceneTextureTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(SceneTextureTagBase, TagBase) };

	struct SceneColorTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SceneColorTextureTag, SceneTextureTagBase) };

	// ポストエフェクト
	struct PostEffectColorTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(PostEffectColorTextureTag, SceneTextureTagBase) };

	// シーンのデプスステンシル
	struct SceneDepthStencilTextureTag final : public SceneTextureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SceneDepthStencilTextureTag, SceneTextureTagBase) };
}