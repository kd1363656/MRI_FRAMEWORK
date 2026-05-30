#include "TextureRecord.h"

bool FWK::Graphics::TextureRecord::PushDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_gpuResource.m_resource,					        "TextureRecordのGPUResourceが無効のため、遅延解放Queueへの登録に失敗しました。",  false)
	FWK_ASSERT_RETURN_VALUE_IF(m_srvStorageID	   == Constant::k_invalidStorageID, "TextureRecordのSRVStorageIDが無効のため、遅延解放Queueへの登録に失敗しました。", false)
	FWK_ASSERT_RETURN_VALUE_IF(a_retiredFenceValue == Constant::k_unusedFenceValue, "FenceValueが無効のため、TextureRecordの遅延解放Queue登録に失敗しました。",	      false)

	// GPUResourceはQueueへ所有権を移す
	// Queue内のRecordが消えるタイミングでComPtrが自然にReleaseされる
	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(m_gpuResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_srvDescriptorIndexReleaseRecord = {};

	l_srvDescriptorIndexReleaseRecord.m_storageID		  = m_srvStorageID;	
	l_srvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushGPUResourceRecord(std::move(l_gpuResourceReleaseRecord)),		 "TextureRecordのGPUResourceを遅延解放Queueへ登録できませんでした。",		 false)
	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushSRVDescriptorIndex(std::move(l_srvDescriptorIndexReleaseRecord)), "TextureRecordのSRVDescriptorIndexを遅延解放Queueへ登録できませんでした。", false)

	// 二解放を防ぐため、Queueへ渡したDescriptorIndexは無効化する
	m_srvStorageID = Constant::k_invalidStorageID;

	return true;
}