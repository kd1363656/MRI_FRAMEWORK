#pragma once

namespace FWK::Tag
{
	// テクスチャのリソースステート遷移用タグ
	struct RenderGraphTextureUsageTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphTextureUsageTagBase, TagBase); };

	struct RenderGraphRenderTargetUsageTag final : public RenderGraphTextureUsageTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphRenderTargetUsageTag, RenderGraphTextureUsageTagBase); };
	struct RenderGraphCopySourceUsageTag   final : public RenderGraphTextureUsageTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphCopySourceUsageTag,   RenderGraphTextureUsageTagBase); };
	struct RenderGraphShaderReadUsageTag   final : public RenderGraphTextureUsageTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphShaderReadUsageTag,   RenderGraphTextureUsageTagBase); };
	struct RenderGraphDepthWriteUsageTag   final : public RenderGraphTextureUsageTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphDepthWriteUsageTag,   RenderGraphTextureUsageTagBase); };
	struct RenderGraphDepthReadUsageTag    final : public RenderGraphTextureUsageTagBase { FWK_DEFINE_TYPE_INFO_TAG(RenderGraphDepthReadUsageTag,    RenderGraphTextureUsageTagBase); };
}