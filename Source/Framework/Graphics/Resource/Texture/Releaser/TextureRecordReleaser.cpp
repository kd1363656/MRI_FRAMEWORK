#include "TextureRecordReleaser.h"

bool FWK::Graphics::TextureRecordReleaser::ReleaseRecord(const std::weak_ptr<Struct::TextureRecord>& a_textureRecord) const
{
	const auto& l_textureRecord = a_textureRecord.lock();

	if (!l_textureRecord)
	{
		assert(false && "テクスチャレコードの解放処理に失敗しました。");
		return false;
	}

	// TextureResourceを解放
	if (l_textureRecord->m_textureResource)
	{
		l_textureRecord->m_textureResource.Reset();
	}

	// SRV用StorageIDを返却する
	if (l_textureRecord->m_srvStorageID != Constant::k_invalidStorageID)
	{
		m_srvDescriptorPool.Release(l_textureRecord->m_srvStorageID);
	}

	return true;
}
