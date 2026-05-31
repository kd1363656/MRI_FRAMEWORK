#include "DeferredResourceReleaseQueue.h"

bool FWK::Graphics::DeferredResourceReleaseQueue::PushGPUResourceRecord(Struct::GPUResourceReleaseRecord&& a_releaseRecord)
{
	FWK_ASSERT_RETURN_VALUE_IF				   (!IsValidGPUResourceReleaseRecord(a_releaseRecord), "GPUResourceRecordが無効のため、遅延解放Queueへの登録に失敗しました。", false)
	m_gpuResourceReleaseRecordList.emplace_back(std::move(a_releaseRecord));

	return true;
}

bool FWK::Graphics::DeferredResourceReleaseQueue::PushRTVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord)
{
	FWK_ASSERT_RETURN_VALUE_IF                        (!IsValidDescriptorIndexReleaseRecord(a_releaseRecord), "RTV用DescriptorIndexReleaseRecordが無効のため、遅延解放Queueへの登録に失敗しました。", false)
	m_rtvDescriptorIndexReleaseRecordList.emplace_back(std::move(a_releaseRecord));

	return true;
}
bool FWK::Graphics::DeferredResourceReleaseQueue::PushSRVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord)
{
	FWK_ASSERT_RETURN_VALUE_IF                        (!IsValidDescriptorIndexReleaseRecord(a_releaseRecord), "SRV用DescriptorIndexReleaseRecordが無効のため、遅延解放Queueへの登録に失敗しました。", false)
	m_srvDescriptorIndexReleaseRecordList.emplace_back(std::move(a_releaseRecord));

	return true;
}
bool FWK::Graphics::DeferredResourceReleaseQueue::PushDSVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord)
{
	FWK_ASSERT_RETURN_VALUE_IF                        (!IsValidDescriptorIndexReleaseRecord(a_releaseRecord), "DSV用DescriptorIndexReleaseRecordが無効のため、遅延解放Queueへの登録に失敗しました。", false)
	m_dsvDescriptorIndexReleaseRecordList.emplace_back(std::move(a_releaseRecord));

	return true;
}

void FWK::Graphics::DeferredResourceReleaseQueue::ReleaseCompleted(const DirectCommandQueue&				a_directCommandQueue, 
																		 DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool, 
																		 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
																		 DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	const auto& l_completedFenceValue = a_directCommandQueue.FetchVALCompletedFenceValue();

	// GPUResource本体を解放
	// ComPTRを保持しているRecordをpop_backすることで、GPUResourceの参照が外れる。
	ReleaseCompletedGPUResources(l_completedFenceValue);

	// DescriptorのStorageIDを、それぞれ対応するDescriptorPoolへ返す、
	// RTV/SRV/DSVは別のDescriptorHeapなので、必ず別々のPoolへ返す
	ReleaseCompletedDescriptorIndices(l_completedFenceValue, m_rtvDescriptorIndexReleaseRecordList, a_rtvDescriptorPool);
	ReleaseCompletedDescriptorIndices(l_completedFenceValue, m_srvDescriptorIndexReleaseRecordList, a_srvDescriptorPool);
	ReleaseCompletedDescriptorIndices(l_completedFenceValue, m_dsvDescriptorIndexReleaseRecordList, a_dsvDescriptorPool);

}

bool FWK::Graphics::DeferredResourceReleaseQueue::IsValidGPUResourceReleaseRecord(const Struct::GPUResourceReleaseRecord& a_releaseRecord) const
{
	if (!a_releaseRecord.m_gpuResource.m_resource)							 { return false; }
	if (a_releaseRecord.m_retiredFenceValue == Constant::k_unusedFenceValue) { return false; }

	return true;
}
bool FWK::Graphics::DeferredResourceReleaseQueue::IsValidDescriptorIndexReleaseRecord(const Struct::DescriptorIndexReleaseRecord& a_releaseRecord) const
{
	if (a_releaseRecord.m_storageID			== Constant::k_invalidStorageID) { return false; }
	if (a_releaseRecord.m_retiredFenceValue == Constant::k_unusedFenceValue) { return false; }

	return true;
}

void FWK::Graphics::DeferredResourceReleaseQueue::ReleaseCompletedGPUResources(const UINT64& a_completedFenceValue)
{
	std::size_t l_index = 0ULL;

	while (l_index < m_gpuResourceReleaseRecordList.size())
	{
		// GPUのフェンス値より大きいフェンス値ならまだ解放しない
		if (m_gpuResourceReleaseRecordList[l_index].m_retiredFenceValue > a_completedFenceValue)
		{
			++l_index;
			continue;
		}

		// 解放順は不要なので、末尾要素を移動してpop_backする
		// pop_backされた要素内のComPtrは自然にReleaseされる
		std::swap(m_gpuResourceReleaseRecordList[l_index], m_gpuResourceReleaseRecordList.back());
		m_gpuResourceReleaseRecordList.pop_back();
	}
}