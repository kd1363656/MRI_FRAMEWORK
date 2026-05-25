#pragma once

namespace FWK::Struct
{
	struct RenderGraphTextureAccess final
	{
		TypeAlias::TypeTag m_textureTag = Constant::k_invalidTypeTag;

		D3D12_RESOURCE_STATES m_requiredState = D3D12_RESOURCE_STATE_COMMON;
	};
}