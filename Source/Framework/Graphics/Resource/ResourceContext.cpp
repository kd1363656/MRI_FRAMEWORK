#include "ResourceContext.h"

void FWK::Graphics::ResourceContext::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_resourceContextJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::ResourceContext::Create(const Device& a_device)
{
	if (!m_rtvDescriptorPool.Create(a_device))
	{
		assert(false && "RTV用ディスクリプタプールの作成処理に失敗しました。");
		return false;
	}

	if (!m_srvDescriptorPool.Create(a_device))
	{
		assert(false && "SRV用ディスクリプタプールの作成処理に失敗しました。");
		return false;
	}

	if (!m_gpuMemoryAllocator.Create(a_device))
	{
		assert(false && "GPUMemoryAllocatorの作成処理に失敗しました。");
		return false;
	}

	if (!m_uploadSystem.Create(a_device))
	{
		assert(false && "アップロードシステムの作成処理に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::ResourceContext::Serialize() const
{
	return m_resourceContextJsonConverter.Serialize(*this);
}