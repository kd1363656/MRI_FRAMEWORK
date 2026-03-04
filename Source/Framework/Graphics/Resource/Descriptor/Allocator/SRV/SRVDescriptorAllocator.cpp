#include "SRVDescriptorAllocator.h"

FWK::Graphics::SRVDescriptorHeapAllocator::SRVDescriptorHeapAllocator(const SRVDescriptorHeap& a_srvDescriptorHeap) : 
	k_srvDescriptorHeap(a_srvDescriptorHeap)
{}
FWK::Graphics::SRVDescriptorHeapAllocator::~SRVDescriptorHeapAllocator() = default;

void FWK::Graphics::SRVDescriptorHeapAllocator::Init()
{
	m_freeIndexList.clear();

	m_nextIndex = 0U;
}

UINT FWK::Graphics::SRVDescriptorHeapAllocator::Allocate()
{
	// 返却済みがあればそれを再利用(LIFO)
	if (!m_freeIndexList.empty())
	{
		const UINT l_index = m_freeIndexList.back();

		// 使用したインデックスをフリーリストから削除する
		m_freeIndexList.pop_back();
		return l_index;
	}

	const UINT l_capacity = k_srvDescriptorHeap.GetCapacity();

	if (m_nextIndex >= l_capacity)
	{
		assert(false && "SRVDescriptorHeapの容量が足りません、容量をもっと確保してください。");
		return 0U;
	}

	const UINT l_alloc = m_nextIndex;

	++m_nextIndex;

	return l_alloc;
}

void FWK::Graphics::SRVDescriptorHeapAllocator::Free(const UINT a_index)
{
	m_freeIndexList.emplace_back(a_index);
}