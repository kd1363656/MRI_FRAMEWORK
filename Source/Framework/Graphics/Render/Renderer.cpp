#include "Renderer.h"

void FWK::Graphics::Renderer::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_rendererJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::Renderer::Create(const Device& a_device)
{
	for (auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource.Create(a_device))
		{
			assert(false && "フレームリソースの作成処理に失敗しました。");
			return false;
		}
	}

	if (!m_directCommandQueue.Create(a_device))
	{
		assert(false && "ダイレクトコマンドキューの作成処理に失敗しました。");
		return false;
	}

	if (!m_directCommandList.Create(a_device))
	{
		assert(false && "ダイレクトコマンドリストの作成処理に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::Renderer::Serialize() const
{
	nlohmann::json l_rootJson = {};

	l_rootJson = m_rendererJsonConverter.Serialize(*this);

	return l_rootJson;
}

void FWK::Graphics::Renderer::AddFrameResource(const FrameResource& a_frameResource)
{
	m_frameResourceList.emplace_back(a_frameResource);
}