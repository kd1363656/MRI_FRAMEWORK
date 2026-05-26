#pragma once

namespace FWK::Tag
{
	// どのパスかを示すタグ
	struct RenderGraphPassTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphPassTagBase, TagBase) };

	struct RenderGraphTestSceneColorWritePassTag final : public RenderGraphPassTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphTestSceneColorWritePassTag, RenderGraphPassTagBase) };
	struct RenderGraphTestSceneColorReadPassTag  final : public RenderGraphPassTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphTestSceneColorReadPassTag,  RenderGraphPassTagBase) };
}