#pragma once

namespace FWK::Tag
{
	struct PipelineStateTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(PipelineStateTagBase, TagBase); };

	struct SpriteStandardPipelineStateTag final : public PipelineStateTagBase{ FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardPipelineStateTag, PipelineStateTagBase); };
}