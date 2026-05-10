#include "StorageIDAllocator.h"

bool FWK::Graphics::TextureIDAllocator::Create(const TypeAlias::TextureID a_textureIDCapacity)
{
	// 無効値を容量として指定された場合は作成失敗とする
	if (a_textureIDCapacity == Constant::k_invalidTextureID)
	{
		assert(false && "テクスチャIDの割り当て可能数を超えています。");
		return false;
	}

	m_textureIDCapacity = a_textureIDCapacity;
	m_nextTextureID     = k_firstNextTextureID;

	// 全スロットを未使用状態で初期化する
	m_isAllocatedList.assign(m_textureIDCapacity, k_unallocatedTextureIDState);

	// キューも何も保持していない状態にする
	m_freeTextureIDQueue = {};

	return true;
}

void FWK::Graphics::TextureIDAllocator::Release(const TypeAlias::TextureID a_textureID)
{
	// 範囲外TextureIDの開放は不正
	if (!IsValidTextureID(a_textureID))
	{
		assert(false && "解放しようとしたTextureIDが確保範囲外です。");
		return;
	}

	// 未使用スロットの二重開放を防ぐ
	if (!m_isAllocatedList[a_textureID])
	{
		assert(false && "未使用のTextureIDを解放しようとしました。");
		return;
	}

	m_isAllocatedList[a_textureID] = k_unallocatedTextureIDState;
	m_freeTextureIDQueue.push(a_textureID);
}

FWK::TypeAlias::TextureID FWK::Graphics::TextureIDAllocator::Allocate()
{
	// 解放済みスロットがあればそれを優先再利用する
	if (!m_freeTextureIDQueue.empty())
	{
		const TypeAlias::TextureID l_reuseTextureID = m_freeTextureIDQueue.front();

		m_freeTextureIDQueue.pop();

		// 有効なインデックスがかどうかを確認
		if (!IsValidTextureID(l_reuseTextureID))
		{
			assert(false && "再利用しようとしたテクスチャIDが確保範囲外です。");
			return Constant::k_invalidTextureID;
		}

		m_isAllocatedList[l_reuseTextureID] = k_allocatedTextureIDState;

		return l_reuseTextureID;
	}

	// 未使用領域が残っているなら新規払い出しする
	if (IsValidTextureID(m_nextTextureID))
	{
		const UINT l_allocateTextureID = m_nextTextureID;

		++m_nextTextureID;

		// 新規払い出しするインデックス番号は割り当て済みにする
		m_isAllocatedList[l_allocateTextureID] = k_allocatedTextureIDState;

		return l_allocateTextureID;
	}

	assert(false && "TextureIDの空きがなくなり、割り当てに失敗しました。");
	return Constant::k_invalidTextureID;
}

bool FWK::Graphics::TextureIDAllocator::IsValidTextureID(const TypeAlias::TextureID a_textureID) const
{
	// 範囲外インデックスを指し示すならfalseを返す
	if (a_textureID >= m_textureIDCapacity ||
		a_textureID >= static_cast<TypeAlias::TextureID>(m_isAllocatedList.size()))
	{
		return false;
	}

	return true;
}