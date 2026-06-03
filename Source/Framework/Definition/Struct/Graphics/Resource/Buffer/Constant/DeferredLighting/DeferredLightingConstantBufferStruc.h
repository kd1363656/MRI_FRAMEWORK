#pragma once

namespace FWK::Struct
{
	struct CBDeferredLightingPass final
	{
		TypeAlias::StorageID m_gBufferAlbedoTextureSRVIndex        = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_gBufferNormalTextureSRVIndex        = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_gBufferWorldPositionTextureSRVIndex = Constant::k_invalidStorageID;

		UINT m_padding = 0U;
	};
}