#include "TextureRecordReleaser.h"

FWK::Graphics::TextureRecordReleaser::TextureRecordReleaser(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) : 
	m_srvDescriptorPool(a_srvDescriptorPool)
{}
FWK::Graphics::TextureRecordReleaser::~TextureRecordReleaser() = default;

bool FWK::Graphics::TextureRecordReleaser::ReleaseRecord(Struct::TextureRecord& a_textureRecord) const
{
	// SRV用StorageIDを返却する
	if (a_textureRecord.m_srvStorageID != Constant::k_invalidStorageID)
	{
		m_srvDescriptorPool.Release(a_textureRecord.m_srvStorageID);

		// 二重開放を防ぐため、返却後は無効値に戻す
		a_textureRecord.m_srvStorageID = Constant::k_invalidStorageID;
	}

	return true;
}
