#include "ResourceContext.h"

void FWK::Graphics::ResourceContext::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_resourceContextJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::ResourceContext::Create(const Device& a_device)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_rtvDescriptorPool.Create(a_device),  "RTVDescriptorPoolの作成処理に失敗しました。",  false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_srvDescriptorPool.Create(a_device),  "SRVDescriptorPoolの作成処理に失敗しました。",  false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_dsvDescriptorPool.Create(a_device),  "DSVDescriptorPoolの作成処理に失敗しました。",  false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_gpuMemoryAllocator.Create(a_device), "GPUMemoryAllocatorの作成処理に失敗しました。", false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_uploadSystem.Create(a_device),	   "UploadSystemの作成処理に失敗しました。",		   false)

	FWK_ASSERT_RETURN_VALUE_IF(!m_textureSystem.Create(a_device,
													   m_gpuMemoryAllocator,
													   m_srvDescriptorPool,
													   m_uploadSystem),
													   "TextureSystemの作成処理に失敗しました。",
													   false)

	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelSystem.Create(), "StaticModelSystemの作成処理に失敗しました。", false)

	return true;
}

void FWK::Graphics::ResourceContext::BeginFrame(const DirectCommandQueue& a_directCommandQueue)
{
	// ロード予約のあったテクスチャを一括ロード
	m_textureSystem.LoadPendingTexturesAndWait(m_uploadSystem);

	// ロード予約のあったStaticModelを一括ロード
	m_staticModelSystem.LoadPendingStaticModelAndWait(m_uploadSystem);

	// 参照カウントが0になったRecordからQueueへ積まれたGPUResource/SRVを、
	// GPUのFence完了後に安全に解放する
	m_deferredResourceReleaseQueue.ReleaseCompleted(a_directCommandQueue, 
													m_rtvDescriptorPool,
													m_srvDescriptorPool,
													m_dsvDescriptorPool);
}

nlohmann::json FWK::Graphics::ResourceContext::Serialize() const
{
	return m_resourceContextJsonConverter.Serialize(*this);
}