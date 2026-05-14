#include "StorageIDAllocator.h"

void FWK::Graphics::StorageIDAllocator::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
}

bool FWK::Graphics::StorageIDAllocator::Create(const TypeAlias::StorageID a_storageIDCapacity)
{
	// 無効値を容量として指定された場合は作成失敗とする
	if (a_storageIDCapacity == Constant::k_invalidStorageIDCapacity)
	{
		assert(false && "ストレージIDの割り当て可能数が0です。");
		return false;
	}

	if (a_storageIDCapacity == Constant::k_invalidStorageID)
	{
		assert(false && "StorageIDの割り当て可能数が無効値です。");
		return false;
	}

	m_storageIDCapacity = a_storageIDCapacity;
	m_nextStorageID     = k_initialNextStorageID;

	// 全スロットを未使用状態で初期化する
	m_isAllocatedList.assign(m_storageIDCapacity, k_unallocatedStorageIDState);

	// キューも何も保持していない状態にする
	m_freeStorageIDQueue = {};

	return true;
}

void FWK::Graphics::StorageIDAllocator::Release(const TypeAlias::StorageID a_storageID)
{
	// 範囲外StorageIDの解放は不正
	if (!IsValidStorageID(a_storageID))
	{
		assert(false && "解放しようとしたStorageIDが確保範囲外です。");
		return;
	}

	// 未使用スロットの二重解放を防ぐ
	if (!m_isAllocatedList[a_storageID])
	{
		assert(false && "未使用のStorageIDを解放しようとしました。");
		return;
	}

	m_isAllocatedList[a_storageID] = k_unallocatedStorageIDState;
	m_freeStorageIDQueue.push(a_storageID);
}

nlohmann::json FWK::Graphics::StorageIDAllocator::Serialize() const
{
	nlohmann::json l_rootJson = {};

	return l_rootJson;
}

FWK::TypeAlias::StorageID FWK::Graphics::StorageIDAllocator::Allocate()
{
	// 解放済みスロットがあればそれを優先再利用する
	if (!m_freeStorageIDQueue.empty())
	{
		const TypeAlias::StorageID l_reuseStorageID = m_freeStorageIDQueue.front();

		m_freeStorageIDQueue.pop();

		// 有効なインデックスかどうかを確認
		if (!IsValidStorageID(l_reuseStorageID))
		{
			assert(false && "再利用しようとしたストレージIDが確保範囲外です。");
			return Constant::k_invalidStorageID;
		}

		m_isAllocatedList[l_reuseStorageID] = k_allocatedStorageIDState;

		return l_reuseStorageID;
	}

	// 未使用領域が残っているなら新規払い出しする
	if (IsValidStorageID(m_nextStorageID))
	{
		const auto l_allocateStorageID = m_nextStorageID;

		++m_nextStorageID;

		// 新規払い出しするインデックス番号は割り当て済みにする
		m_isAllocatedList[l_allocateStorageID] = k_allocatedStorageIDState;

		return l_allocateStorageID;
	}

	assert(false && "StorageIDの空きがなくなり、割り当てに失敗しました。");
	return Constant::k_invalidStorageID;
}

bool FWK::Graphics::StorageIDAllocator::IsValidStorageID(const TypeAlias::StorageID a_storageID) const
{
	// 範囲外インデックスを指し示すならfalseを返す
	if (a_storageID >= m_storageIDCapacity ||
		a_storageID >= static_cast<TypeAlias::StorageID>(m_isAllocatedList.size()))
	{
		return false;
	}

	return true;
}