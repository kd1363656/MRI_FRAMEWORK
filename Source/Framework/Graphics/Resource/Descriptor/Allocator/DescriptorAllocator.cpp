#include "DescriptorAllocator.h"

void FWK::Graphics::DescriptorAllocator::Init()
{
	m_capacity  = 0U;
	m_nextIndex = 0U;

	m_isAllocatedList.clear();

	m_freeIndexQueue = std::queue<UINT>();
}
bool FWK::Graphics::DescriptorAllocator::Create(const UINT a_capacity)
{
	// 0個の確保は意味がないため失敗扱いとする
	if (a_capacity == 0U)
	{
		assert(false && "ディスクリプタ割り当て数が0で作成できませんでした。");
		return false;
	}

	m_capacity  = a_capacity;
	m_nextIndex = 0U;

	// 全スロットを未使用状態で初期化する
	m_isAllocatedList.assign(a_capacity, false);

	m_freeIndexQueue = std::queue<UINT>();

	return true;
}

UINT FWK::Graphics::DescriptorAllocator::Allocate()
{
	// 解放済みスロットがあればそれを優先再利用する
	if (!m_freeIndexQueue.empty())
	{
		const UINT l_reuseIndex = m_freeIndexQueue.front();
		m_freeIndexQueue.pop();
		
		if (l_reuseIndex >= m_capacity)
		{
			assert(false && "再利用しようとしたディスクリプタインデックスが確保範囲外です。");
			return k_invalidDescriptorIndex;
		}

		m_isAllocatedList[l_reuseIndex] = true;

		return l_reuseIndex;
	}

	// 未使用領域が残っているなら新規払い出しする
	if (m_nextIndex < m_capacity)
	{
		const UINT l_allocateIndex = m_nextIndex;

		++m_nextIndex;

		m_isAllocatedList[l_allocateIndex] = true;

		return l_allocateIndex;
	}

	assert(false && "ディスクリプタヒープの空きがなくなり、割り当てに失敗しました。");
	return k_invalidDescriptorIndex;
}

void FWK::Graphics::DescriptorAllocator::Release(const UINT a_index)
{
	// 範囲外インデックスの解放は不正
	if (a_index >= m_capacity)
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

	m_isAllocatedList[a_index] = false;
	m_freeIndexQueue.push(a_index);
}

bool FWK::Graphics::DescriptorAllocator::IsAllocated(const UINT a_index) const
{
	if (a_index >= m_capacity)
	{
		return false;
	}

	return m_isAllocatedList[a_index];
}