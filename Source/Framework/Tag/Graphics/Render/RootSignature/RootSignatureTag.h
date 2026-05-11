#pragma once

namespace FWK::Tag
{
	struct RootSignatureTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootSignatureTagBase, TagBase); };

	struct SpriteStandardRootSignatureTag final : public RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardRootSignatureTag, RootSignatureTagBase); };
}