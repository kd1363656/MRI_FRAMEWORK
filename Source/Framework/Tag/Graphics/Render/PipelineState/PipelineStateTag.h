#pragma once

namespace FWK::Tag
{
	struct PipelineStateTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(PipelineStateTagBase, TagBase) };

	// テクスチャ用パイプライン
	struct SpriteStandardPipelineStateTag final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardPipelineStateTag, PipelineStateTagBase) };

	// モデル用パイプライン
	struct ModelUnLitStandardPipelineStateTag      final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelUnLitStandardPipelineStateTag,	   PipelineStateTagBase) };
	struct ModelLitStandardPipelineStateTag        final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelLitStandardPipelineStateTag,		   PipelineStateTagBase) };
	struct ModelLitGBufferStandardPipelineStateTag final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelLitGBufferStandardPipelineStateTag, PipelineStateTagBase) };
	
	// ポストエフェクト用パイプライン
	struct PostEffectPipelineStateTag final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(PostEffectPipelineStateTag, PipelineStateTagBase) };

	// 最終出力テクスチャ用パイプライン
	struct FinalPresentPipelineStateTag	final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(FinalPresentPipelineStateTag, PipelineStateTagBase) };
}