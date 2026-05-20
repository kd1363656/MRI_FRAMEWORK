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
		assert(false && "RTVDescriptorPoolの作成処理に失敗しました。");
		return false;
	}

	if (!m_srvDescriptorPool.Create(a_device))
	{
		assert(false && "SRVDescriptorPoolの作成処理に失敗しました。");
		return false;
	}

	if (!m_dsvDescriptorPool.Create(a_device))
	{
		assert(false && "DSVDescriptorPoolの作成処理に失敗しました。");
		return false;
	}

	if (!m_gpuMemoryAllocator.Create(a_device))
	{
		assert(false && "GPUMemoryAllocatorの作成処理に失敗しました。");
		return false;
	}

	if (!m_uploadSystem.Create(a_device))
	{
		assert(false && "UploadSystemの作成処理に失敗しました。");
		return false;
	}

	if (!m_textureSystem.Create(a_device,
								m_gpuMemoryAllocator,
								m_srvDescriptorPool,
								m_uploadSystem))
	{
		assert(false && "TextureSystemの作成処理に失敗しました。");
		return false;
	}

	if (!m_staticModelSystem.Create())
	{
		assert(false && "StaticModelSystemの作成処理に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::ResourceContext::BeginFrame(const DirectCommandQueue& a_directCommandQueue)
{
	// ロード予約のあったテクスチャを一括ロード
	m_textureSystem.LoadPendingTexturesAndWait(m_uploadSystem);

	// ロード予約のあったStaticModelを一括ロード
	m_staticModelSystem.LoadPendingStaticModelAndWait(m_uploadSystem);

	// 参照カウントが0で削除すべきテクスチャを削除
	m_textureSystem.ReleaseCompletedUnusedTexture(a_directCommandQueue, m_srvDescriptorPool);

	// 参照カウントが0で削除すべきStaticModelを削除
	m_staticModelSystem.ReleaseCompletedUnusedStaticModel(a_directCommandQueue, m_srvDescriptorPool);
}

nlohmann::json FWK::Graphics::ResourceContext::Serialize() const
{
	return m_resourceContextJsonConverter.Serialize(*this);
}