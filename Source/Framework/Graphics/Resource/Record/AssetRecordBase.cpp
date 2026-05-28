#include "AssetRecordBase.h"

void FWK::Graphics::AssetRecordBase::AddReferenceCount()
{
	++m_referenceCount;
}

bool FWK::Graphics::AssetRecordBase::SubReferenceCount()
{
	if (m_referenceCount == Constant::k_emptyAssetReferenceCount)
	{
		assert(false && "参照数が0のAssetRecordに対して参照数の減算が行われました。");
		return false;
	}

	--m_referenceCount;

	return true;
}

bool FWK::Graphics::AssetRecordBase::IsUnused() const
{
	return m_referenceCount == Constant::k_emptyAssetReferenceCount;
}

void FWK::Graphics::AssetRecordBase::InvalidateStorageID()
{
	m_storageID = Constant::k_invalidStorageID;
}