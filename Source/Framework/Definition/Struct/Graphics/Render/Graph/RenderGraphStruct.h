#pragma once

namespace FWK::Struct
{
	struct RenderGraphTextureAccess final
	{
		TypeAlias::TypeTag m_textureTag = Constant::k_invalidTypeTag;

		TypeAlias::TypeTag m_accessTag = Constant::k_invalidTypeTag;

		TypeAlias::TypeTag m_usageTag = Constant::k_invalidTypeTag;
	};
}