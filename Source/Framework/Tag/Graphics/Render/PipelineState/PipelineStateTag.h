#pragma once

namespace FWK::Tag
{
	struct PipelineStateTagBase : TagBase { FWK_DEFINE_TYPE_INFO_TAG(PipelineStateTagBase, TagBase); };

	struct SpriteStandardPipelineStateTag final : PipelineStateTagBase{ FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardPipelineStateTag, PipelineStateTagBase); };
}