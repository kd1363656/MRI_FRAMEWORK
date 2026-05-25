#pragma once

namespace FWK::Tag
{
	struct RenderGraphAccessTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphAccessTagBase, TagBase) };

	struct RenderGraphReadAccessTag  final : public RenderGraphAccessTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphReadAccessTag,  RenderGraphAccessTagBase) };
	struct RenderGraphWriteAccessTag final : public RenderGraphAccessTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphWriteAccessTag, RenderGraphAccessTagBase) };
}