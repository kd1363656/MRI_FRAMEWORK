#pragma once

namespace FWK::Graphics
{
	class DeferredResourceReleaseQueue
	{
	public:

		 DeferredResourceReleaseQueue() = default;
		~DeferredResourceReleaseQueue() = default;

		bool PushGPUResourceRecord (Struct::GPUResourceReleaseRecord&& a_releaseRecord);

		bool PushRTVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord);
		bool PushSRVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord);
		bool PushDSVDescriptorIndex(Struct::DescriptorIndexReleaseRecord&& a_releaseRecord);
		
		void ReleaseCompleted(const DirectCommandQueue&				   a_directCommandQueue, 
								    DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
									DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
									DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

	private:

		bool IsValidGPUResourceReleaseRecord    (const Struct::GPUResourceReleaseRecord&	 a_releaseRecord) const;
		bool IsValidDescriptorIndexReleaseRecord(const Struct::DescriptorIndexReleaseRecord& a_releaseRecord) const;

		void ReleaseCompletedGPUResources(const UINT64& a_completedFenceValue);

		template <Concept::IsDerivedDescriptorHeapBaseConcept DescriptorHeapType>
		void ReleaseCompletedDescriptorIndices(const UINT64& a_completedFenceValue, std::vector<Struct::DescriptorIndexReleaseRecord>& a_releaseRecordList, DescriptorPool<DescriptorHeapType>& a_descriptorPool)
		{
			std::size_t l_index = 0ULL;

			while (l_index < a_releaseRecordList.size())
			{
				// GPUのフェンス値よりも大きいフェンス値ならまだ解放しない
				if (a_releaseRecordList[l_index].m_retiredFenceValue > a_completedFenceValue)
				{
					++l_index;
					continue;
				}

				// 該当するディスクリプタヒープから解放する
				if (a_releaseRecordList[l_index].m_storageID != Constant::k_invalidStorageID)
				{
					a_descriptorPool.Release(a_releaseRecordList[l_index].m_storageID);
				}

				// 解放順は不要なので、末尾要素を移動してpop_backする(要素削除時に動作が速いため)
				// 要素を進めないことでswap後の末尾の要素も確認できるため。
				std::swap(a_releaseRecordList[l_index], a_releaseRecordList.back());
				a_releaseRecordList.pop_back();
			}
		}

		std::vector<Struct::GPUResourceReleaseRecord> m_gpuResourceReleaseRecordList = {};

		std::vector<Struct::DescriptorIndexReleaseRecord> m_rtvDescriptorIndexReleaseRecordList = {};
		std::vector<Struct::DescriptorIndexReleaseRecord> m_srvDescriptorIndexReleaseRecordList = {};
		std::vector<Struct::DescriptorIndexReleaseRecord> m_dsvDescriptorIndexReleaseRecordList = {};
	};
}