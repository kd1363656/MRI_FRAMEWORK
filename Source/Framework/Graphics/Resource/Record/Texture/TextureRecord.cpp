#include "TextureRecord.h"

bool FWK::Graphics::TextureRecord::PushDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	if (!m_gpuResource.m_resource)
	{
		assert(false && "TextureRecordのGPUResourceが無効のため、遅延解放Queueへの登録に失敗しました。");
		return false;
	}

	if (m_srvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "TextureRecordのSRVStorageIDが無効のため、遅延解放Queueへの登録に失敗しました。");
		return false;
	}

	if (a_retiredFenceValue == Constant::k_unusedFenceValue)
	{
		assert(false && "FenceValueが無効のため、TextureRecordの遅延解放Queue登録に失敗しました。");
		return false;
	}

	// GPUResourceはQueueへ所有権を移す
	// Queue内のRecordが消えるタイミングでComPtrが自然にReleaseされる
	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(m_gpuResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_srvDescriptorIndexReleaseRecord = {};

	l_srvDescriptorIndexReleaseRecord.m_storageID		  = m_srvStorageID;	
	l_srvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	if (!a_deferredResourceReleaseQueue.PushGPUResourceRecord(std::move(l_gpuResourceReleaseRecord)))
	{
		assert(false && "TextureRecordのGPUResourceを遅延解放Queueへ登録できませんでした。");
		return false;
	}

	if (!a_deferredResourceReleaseQueue.PushSRVDescriptorIndex(std::move(l_srvDescriptorIndexReleaseRecord)))
	{
		assert(false && "TextureRecordのSRVDescriptorIndexを遅延解放Queueへ登録できませんでした。");
		return false;
	}

	// 二解放を防ぐため、Queueへ渡したDescriptorIndexは無効化する
	m_srvStorageID = Constant::k_invalidStorageID;

	return true;
}