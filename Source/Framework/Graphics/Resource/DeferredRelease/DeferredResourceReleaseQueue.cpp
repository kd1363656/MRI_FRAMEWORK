#include "DeferredResourceReleaseQueue.h"

bool FWK::Graphics::DeferredResourceReleaseQueue::PushGPUResourceRecord(Struct::GPUResourceReleaseRecord&& a_releaseRecord)
{
	if (!IsValidGPUResourceReleaseRecord(a_releaseRecord))
	{
		assert(false && "GPUResourceRecordが無効のため、遅延解放Queueへの登録に失敗しました。");
		return false;
	}

	m_gpuResourceReleaseRecordList.emplace_back(std::move(a_releaseRecord));

	return true;
}
bool FWK::Graphics::DeferredResourceReleaseQueue::PushSRVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord)
{
	if (!IsValidDescriptorIndexReleaseRecord(a_releaseRecord))
	{
		assert(false && "SRV用DescriptorIndexReleaseRecordが無効のため、遅延解放Queueへの登録に失敗しました。");
		return false;
	}

	m_srvDescriptorIndexReleaseRecordList.emplace_back(std::move(a_releaseRecord));

	return true;
}

void FWK::Graphics::DeferredResourceReleaseQueue::ReleaseCompleted(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	const auto& l_completedFenceValue = a_directCommandQueue.FetchVALCompletedFenceValue();

	ReleaseCompletedGPUResource(l_completedFenceValue);

	ReleaseCompletedDescriptorIndices(l_completedFenceValue, m_srvDescriptorIndexReleaseRecordList, a_srvDescriptorPool);
}

bool FWK::Graphics::DeferredResourceReleaseQueue::IsValidGPUResourceReleaseRecord(const Struct::GPUResourceReleaseRecord& a_releaseRecord) const
{
	if (!a_releaseRecord.m_gpuResource.m_resource) { return false; }

	if (a_releaseRecord.m_retiredFenceValue == Constant::k_unusedFenceValue) { return false; }

	return true;
}
bool FWK::Graphics::DeferredResourceReleaseQueue::IsValidDescriptorIndexReleaseRecord(const Struct::DescriptorIndexReleaseRecord& a_releaseRecord) const
{
	if (a_releaseRecord.m_storageID == Constant::k_invalidStorageID) { return false; }

	if (a_releaseRecord.m_retiredFenceValue == Constant::k_unusedFenceValue) { return false; }

	return true;
}

void FWK::Graphics::DeferredResourceReleaseQueue::ReleaseCompletedGPUResource(const UINT64& a_completedFenceValue)
{
	std::size_t l_index = 0ULL;

	while (l_index < m_gpuResourceReleaseRecordList.size())
	{
		// GPUのフェンス値より大木フェンス値ならまだ解放しない
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