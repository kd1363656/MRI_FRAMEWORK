#pragma once

namespace FWK::Tag
{
	struct PipelineStateTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(PipelineStateTagBase, TagBase); };

	struct FinalPresentPipelineStateTag		  final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(FinalPresentPipelineStateTag,       PipelineStateTagBase); };
	struct SpriteStandardPipelineStateTag     final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardPipelineStateTag,     PipelineStateTagBase); };
	struct ModelUnLitStandardPipelineStateTag final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelUnLitStandardPipelineStateTag, PipelineStateTagBase); };
	struct ModelLitStandardPipelineStateTag   final : public PipelineStateTagBase { FWK_DEFINE_TYPE_INFO_TAG(ModelLitStandardPipelineStateTag,   PipelineStateTagBase); };
}