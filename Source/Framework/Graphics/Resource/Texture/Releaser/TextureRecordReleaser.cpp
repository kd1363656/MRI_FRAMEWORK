#include "TextureRecordReleaser.h"

bool FWK::Graphics::TextureRecordReleaser::ReleaseRecord(Struct::TextureRecord& a_textureRecord) const
{
	// TextureResourceを解放
	if (a_textureRecord.m_textureResource)
	{
		a_textureRecord.m_textureResource.Reset();
	}

	// SRV用StorageIDを返却する
	if (a_textureRecord.m_srvStorageID != Constant::k_invalidStorageID)
	{
		m_srvDescriptorPool.Release(a_textureRecord.m_srvStorageID);
	}

	return true;
}