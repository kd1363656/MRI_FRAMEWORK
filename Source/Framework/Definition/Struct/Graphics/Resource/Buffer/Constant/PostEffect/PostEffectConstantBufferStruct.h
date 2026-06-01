#pragma once

namespace FWK::Struct
{
	struct CBPostEffectPass final
	{
		TypeAlias::StorageID	 m_sourceTextureSRVIndex = Constant::k_invalidStorageID;
		TypeAlias::Math::Vector3 m_padding				 = {};
	};
}