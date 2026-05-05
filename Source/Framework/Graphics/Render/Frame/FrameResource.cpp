#include "FrameResource.h"

void FWK::Graphics::FrameResource::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_frameResourceJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::FrameResource::Create(const Device& a_device)
{
	if (!m_directCommandAllocator.Create(a_device))
	{
		assert(false && "ダイレクトコマンドアロケータの作成処理に失敗しました。");
		return false;
	}

	for (const auto& [l_staticTypeID, l_constantBuffer] : m_constantBufferMap)
	{
		if (!l_constantBuffer) { continue; }

		l_constantBuffer->Create(a_device);
	}

	return true;
}

nlohmann::json FWK::Graphics::FrameResource::Serialize() const
{
	return m_frameResourceJsonConverter.Serialize(*this);
}

void FWK::Graphics::FrameResource::AddConstantBuffer(const std::shared_ptr<ConstantBufferBase>& a_constantBuffer)
{
	if (!a_constantBuffer) { return; }

	const auto l_staticTypeID = a_constantBuffer->GetRuntimeTypeINFO().k_staticTypeID;

	m_constantBufferMap.try_emplace(l_staticTypeID, a_constantBuffer);
}