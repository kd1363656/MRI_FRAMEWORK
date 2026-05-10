#pragma once

namespace FWK::Struct
{
	struct AssetRecordBase
	{
		std::wstring m_filePath = {};

		UINT64 m_retiredFenceValue = Constant::k_unusedFenceValue;

		UINT m_referenceCount = Constant::k_emptyAssetReferenceCount;

		TypeAlias::StorageID m_storageID = Constant::k_invalidStorageID;
	};
}