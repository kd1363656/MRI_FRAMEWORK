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

	if (!m_srvCPUDescriptorPool.Create(a_device))
	{
		assert(false && "CPU用ディスクリプタプールの作成処理に失敗しました。");
		return false;
	}

	if (!m_srvShaderVisibleDescriptorPool.Create(a_device))
	{
		assert(false && "ShaderVisible用SRVディスクリプタプールの作成処理に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::ResourceContext::Serialize() const
{
	return m_resourceContextJsonConverter.Serialize(*this);
}