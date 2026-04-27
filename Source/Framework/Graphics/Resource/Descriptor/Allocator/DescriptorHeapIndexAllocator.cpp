#include "DescriptorHeapIndexAllocator.h"

bool FWK::Graphics::DescriptorHeapIndexAllocator::Create(const UINT a_descriptorCapacity)
{
	// 0個の確保は意味がないため失敗扱いとする
	if (a_descriptorCapacity == Constant::k_invalidDescriptorCapacity)
	{
		assert(false && "ディスクリプタ割り当て数が0で作成できませんでした。");
		return false;
	}

	m_descriptorCapacity = a_descriptorCapacity;
	m_nextIndex			 = k_firstDescriptorHeapIndex;
	
	// 全スロットを未使用状態で初期化する
	m_isAllocatedList.assign(m_descriptorCapacity, k_unallocatedDescriptorState);

	// キューも何も保持していない状態にする
	m_freeIndexQueue = {};

	return true;
}

void FWK::Graphics::DescriptorHeapIndexAllocator::Release(const UINT a_index)
{
	// 範囲外インデックスの開放は不正
	if (!IsValidIndex(a_index))
	{
		assert(false && "解放しようとしたインデックスが確保範囲外です。");
		return;
	}

	// 未使用スロットの二重開放を防ぐ
	if (!m_isAllocatedList[a_index])
	{
		assert(false && "未使用のインデックスを解放しようとしました。");
		return;
	}

	m_isAllocatedList[a_index] = k_unallocatedDescriptorState;
	m_freeIndexQueue.push(a_index);
}

UINT FWK::Graphics::DescriptorHeapIndexAllocator::Allocate()
{
	// 解放済みスロットがあればそれを優先再利用する
	if (!m_freeIndexQueue.empty())
	{
		const UINT l_reuseIndex = m_freeIndexQueue.front();

		m_freeIndexQueue.pop();

		// 有効なインデックスがかどうかを確認
		if (!IsValidIndex(l_reuseIndex))
		{
			assert(false && "再利用しようとしたディスクリプタインデックスが確保範囲外です。");
			return Constant::k_invalidDescriptorHeapIndex;
		}

		m_isAllocatedList[l_reuseIndex] = k_allocatedDescriptorState;

		return l_reuseIndex;
	}

	// 未使用領域が残っているなら新規払い出しする
	if (IsValidIndex(m_nextIndex))
	{
		const UINT l_allocateIndex = m_nextIndex;

		++m_nextIndex;

		// 新規払い出しするインデックス番号は割り当て済みにする
		m_isAllocatedList[l_allocateIndex] = k_allocatedDescriptorState;

		return l_allocateIndex;
	}

	assert(false && "ディスクリプタヒープの空きがなくなり、割り当てに失敗しました。");
	return Constant::k_invalidDescriptorHeapIndex;
}

bool FWK::Graphics::DescriptorHeapIndexAllocator::IsValidIndex(const UINT a_index) const
{
	// 範囲外インデックスを指し示すならfalseを返す
	if (a_index >= m_descriptorCapacity ||
		a_index >= static_cast<UINT>(m_isAllocatedList.size()))
	{
		return false;
	}

	return true;
}