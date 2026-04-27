#include "HeapPageAllocatorBase.h"

FWK::Graphics::HeapPageAllocatorBase::HeapPageAllocatorBase(const D3D12_HEAP_TYPE a_createHeapType, const D3D12_HEAP_FLAGS a_createHeapFlags) : 
	k_createHeapType(a_createHeapType),

	k_createHeapFlags(a_createHeapFlags)
{}
FWK::Graphics::HeapPageAllocatorBase::~HeapPageAllocatorBase() = default;

bool FWK::Graphics::HeapPageAllocatorBase::Allocate(const UINT64&                a_allocationSize,
													const UINT64&                a_alignment,
													const Device&                a_device,
														  UINT64&                a_heapOffset,
														  TypeAlias::HeapPageID& a_heapPageID)
{
	a_heapPageID = Constant::k_invalidHeapPageID;
	a_heapOffset = k_initialOffset;

	if (a_allocationSize == 0ULL)
	{
		assert(false && "確保サイズが0のため、HeapPageAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	if (a_alignment == 0ULL)
	{
		assert(false && "アライメントが0のため、HeapPageAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	if (m_heapPageSize == 0ULL)
	{
		assert(false && "既定のヒープページサイズが未設定のため、HeapPageAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	// まず既存ページを先頭から順に調べ、空きページがあればそこへ確保する
	for (auto& l_heapPage : m_heapPageList)
	{
		UINT64 l_heapOffset = 0ULL;

		// 注意l_heapOffsetは参照渡しなので値がこの関数を通じて書き換わる
		if (!l_heapPage.Allocate(a_allocationSize, a_alignment, l_heapOffset)) { continue; }

		a_heapPageID = l_heapPage.GetVALHeapPageID();
		a_heapOffset = l_heapOffset;

		return true;
	}

	// 既存ページに入らない場合は、新しいページを作ってから確保する
	if (!CreateHeapPage(a_device, a_allocationSize, a_alignment))
	{
		assert(false && "新規HeapPageの作成に失敗したため、HeapPageAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	if (m_heapPageList.empty())
	{
		assert(false && "HeapPage追加後にページリストが空のため、HeapPageAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	auto& l_heapPage = m_heapPageList.back();

	UINT64 l_heapOffset = 0ULL;

	if (!l_heapPage.Allocate(a_allocationSize, a_alignment, l_heapOffset))
	{
		assert(false && "新規HeapPageから領域確保処理に失敗しました。");
		return false;
	}

	a_heapPageID = l_heapPage.GetVALHeapPageID();
	a_heapOffset = l_heapOffset;

	return true;
}

bool FWK::Graphics::HeapPageAllocatorBase::Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize, TypeAlias::HeapPageID a_heapPageID)
{
	if (a_heapPageID == Constant::k_invalidHeapPageID)
	{
		assert(false && "無効なHeapPageIDが指定されたため、HeapPageAllocatorの領域解放処理に失敗しました。");
		return false;
	}

	if (a_allocationSize == 0ULL)
	{
		assert(false && "解放対象のサイズが0のため、HeapPageAllocatorの領域解放処理に失敗しました。");
		return false;
	}

	auto* const l_heapPage = FindMutablePTRHeapPage(a_heapPageID);

	if (!l_heapPage)
	{
		assert(false && "解放対象のHeapPageが見つからないため、HeapPageAllocatorの領域解放処理に失敗しました。");
		return false;
	}

	if (!l_heapPage->Release(a_heapOffset, a_allocationSize))
	{
		assert(false && "HeapPageの領域解放処理に失敗しました");
		return false;
	}

	return true;
}

FWK::Graphics::HeapPage* FWK::Graphics::HeapPageAllocatorBase::FindMutablePTRHeapPage(const TypeAlias::HeapPageID a_heapPageID)
{
	const auto& l_itr = std::find_if(m_heapPageList.begin(), m_heapPageList.end(), [a_heapPageID](const HeapPage& a_heapPage) 
	{
		return a_heapPage.GetVALHeapPageID() == a_heapPageID;
	});

	if (l_itr == m_heapPageList.end()) { return nullptr; }

	return std::to_address(l_itr);
}

bool FWK::Graphics::HeapPageAllocatorBase::CreateHeapPage(const Device& a_device, const UINT64& a_allocationSize, const UINT64& a_alignment)
{
	if (a_allocationSize == 0ULL)
	{
		assert(false && "確保サイズが0のため、HeapPage追加作成処理に失敗しました。");
		return false;
	}

	if (a_alignment == 0ULL)
	{
		assert(false && "アライメントが0のため、HeapPage追加作成に失敗しました。");
		return false;
	}

	if (m_heapPageSize == 0ULL)
	{
		assert(false && "基底のヒープページサイズが未設定のため、HeapPage追加作成処理に失敗しました。");
		return false;
	}

	// 例えば
	// allocationSize = 300
	// alignment      = 256
	// の場合、ページ先頭ではなく途中のFreeBlockに置くときは
	// 最大で255バイト切り上げられる可能性がある。
	// そのため最低でも
	// allocationSize + (alignment - 1)
	// を見ておけば、最悪の切り上げが発生しても1個は配置できる
	const UINT64& l_alignmentPaddingMax = a_alignment - 1ULL;

	if (a_allocationSize > (std::numeric_limits<UINT64>::max() - l_alignmentPaddingMax))
	{
		assert(false && "必要ヒープページサイズ計算中にオーバーフローの可能性があるため、HeapPage追加作成処理に失敗しました。");
		return false;
	}

	const UINT64& l_minimumRequiredPageSize = a_allocationSize + l_alignmentPaddingMax;
	const UINT64& l_createHeapPageSize      = std::max(m_heapPageSize, l_minimumRequiredPageSize);

	const TypeAlias::HeapPageID l_heapPageID = GenerateHeapPageID();

	if (l_heapPageID == Constant::k_invalidHeapPageID)
	{
		assert(false && "HeapPageIDの採番に失敗したため、HeapPage追加作成処理に失敗しました。");
		return false;
	}

	HeapPage l_heapPage = {};

	if (!l_heapPage.Create(l_createHeapPageSize,
						   k_createHeapType,
						   k_createHeapFlags,
						   a_device,
						   l_heapPageID))
	{
		assert(false && "HeapPageの作成処理に失敗したため、HeapPage追加作成処理に失敗しました。");
		return false;
	}

	m_heapPageList.emplace_back(std::move(l_heapPage));

	return true;
}

FWK::TypeAlias::HeapPageID FWK::Graphics::HeapPageAllocatorBase::GenerateHeapPageID()
{
	const TypeAlias::HeapPageID l_nextHeapPageID = m_nextHeapPageID;

	if (l_nextHeapPageID == Constant::k_invalidHeapPageID)
	{
		assert(false && "HeapPageID採番中にオーバーフローが発生しました。");
		return Constant::k_invalidHeapPageID;
	}

	// ヒープIDの更新
	++m_nextHeapPageID;

	return m_nextHeapPageID;
}