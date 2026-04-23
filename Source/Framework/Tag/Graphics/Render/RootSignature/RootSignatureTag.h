#pragma once

namespace FWK::Tag
{
	struct RootSignatureTagBase : TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootSignatureTagBase, TagBase); };

	struct SpriteStandardRootSignatureTag final : RootSignatureTagBase { FWK_DEFINE_TYPE_INFO_TAG(SpriteStandardRootSignatureTag, RootSignatureTagBase); };
}