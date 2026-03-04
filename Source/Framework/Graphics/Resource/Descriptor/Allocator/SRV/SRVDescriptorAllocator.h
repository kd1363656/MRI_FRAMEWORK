#pragma once

namespace FWK::Graphics
{
	// SRVDescriptorHeapから「使っていいスロット番号」を払いだすクラス
	class SRVDescriptorHeapAllocator final
	{
	public:

		explicit SRVDescriptorHeapAllocator(const SRVDescriptorHeap& a_srvDescriptorHeap);
		~SRVDescriptorHeapAllocator        ();

		void Init();

		// SRVスロットを一つ確保して「index」を返す
		// indexはDescriptorHeapBase::FetchCPUHandle/FetchGPUHandleに渡すための番号
		UINT Allocate();

		// 使い終わったindexを返却
		void Free(const UINT a_index);

	private:

		const SRVDescriptorHeap& k_srvDescriptorHeap;

		std::vector<UINT> m_freeIndexList;

		UINT m_nextIndex = 0U;
	};
}