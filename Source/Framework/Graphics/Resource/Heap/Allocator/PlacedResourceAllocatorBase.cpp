#include "PlacedResourceAllocatorBase.h"

FWK::Graphics::PlacedResourceAllocatorBase::PlacedResourceAllocatorBase(const D3D12_HEAP_TYPE a_createHeapType, const D3D12_HEAP_FLAGS a_createHeapFlags) : 
	k_createHeapType(a_createHeapType),

	k_createHeapFlags(a_createHeapFlags)
{}
FWK::Graphics::PlacedResourceAllocatorBase::~PlacedResourceAllocatorBase() = default;

bool FWK::Graphics::PlacedResourceAllocatorBase::CreatePlacedResource(const D3D12_RESOURCE_DESC&					a_resourceDesc, 
																	  const Device&								    a_device, 
																	  const D3D12_CLEAR_VALUE*					    a_clearValue,
																	  const D3D12_RESOURCE_STATES					a_initialResourceState,
																	  	    TypeAlias::ComPtr<ID3D12Resource2>&     a_resource, 
																	  	    Struct::PlacedResourceAllocationRecord& a_allocationRecord)
{
	// 初期化
	a_resource.Reset();
	a_allocationRecord = {};

	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されていないため、PlacedResource作成処理に失敗しました。");
		return false;
	}

	// このResourceDescのリソースをHeapに配置するには何バイト必要で、
	// 何バイト境界に配置する必要があるかを調べる関数
	// GetResourceAllocationInfo(対象GPUノード、
	//							 リソース設定数、
	//							 リソース設定配列、	
	//							 指定したResourceをHeap内に配置するために必要なサイズとアライメントを取得する);
	const auto l_allocationINFO = l_device->GetResourceAllocationInfo(GraphicsManager::GetVALDefaultGPUNodeMask(), k_resourceDescCount, &a_resourceDesc);

	if (l_allocationINFO.SizeInBytes == 0ULL)
	{
		assert(false && "Resourceの必要確保サイズが0のため、PlacedResource作成処理に失敗しました。");
		return false;
	}

	if (l_allocationINFO.Alignment == 0ULL)
	{
		assert(false && "Resourceの必要アライメントが0のため、PlacedResource作成処理に失敗しました。");
		return false;
	}

	UINT64 l_heapOffset = 0ULL;

	auto l_heapPageID = Constant::k_invalidHeapPageID;

	// Resourceを配置するために必要なHeap領域を確保し、
	// 配置先HeapPageIDとHeap内Offsetを取得する
	if (!Allocate(l_allocationINFO.SizeInBytes,
				  l_allocationINFO.Alignment,
				  a_device,
				  l_heapOffset,
				  l_heapPageID))
	{
		assert(false && "PlacedResourceを配置するHeap領域の確保に失敗したため、PlacedResource作成処理に失敗しました。");
		return false;
	}

	// HeapPageIDに対応するHeapPageを取得する
	const auto* const l_heapPage = FindPTRHeapPage(l_heapPageID);

	if (!l_heapPage)
	{
		Release(l_heapOffset, l_allocationINFO.SizeInBytes, l_heapPageID);

		assert(false && "作成先HeapPageが見つからないため、PlacedResource作成処理に失敗しました。");
		return false;
	}

	const auto& l_heap = l_heapPage->GetREFHeap();

	if (!l_heap)
	{
		Release(l_heapOffset, l_allocationINFO.SizeInBytes, l_heapPageID);

		assert(false && "作成先HeapPageが無効のため、PlacedResource作成処理に失敗しました。");
		return false;
	}

	// 既存のHeap上の指定オフセット位置にリソースを作成する
	// CreatePlacedResource(配置先となるID3D12Heap、
	//						Heap先頭からの配置開始位置、
	//						resourceのAlignmentを満たす必要がある、
	//						作成するリソースの設定情報、
	//						作成直後の初期リソース状態、
	//						最適化クリア値);
	const auto l_hr = l_device->CreatePlacedResource(l_heap.Get(),
													 l_heapOffset,
													 &a_resourceDesc,
													 a_initialResourceState,
													 a_clearValue,
													 IID_PPV_ARGS(a_resource.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		Release(l_heapOffset, l_allocationINFO.SizeInBytes, l_heapPageID);

		assert(false && "CreatePlacedResourceによるResource作成に失敗しました。");
		return false;
	}

	// PlacedResourceの解放時に必要になる配置情報を記録
	a_allocationRecord.m_heapPageID     = l_heapPageID;
	a_allocationRecord.m_heapOffset     = l_heapOffset;
	a_allocationRecord.m_allocationSize = l_allocationINFO.SizeInBytes;

	return true;
}

bool FWK::Graphics::PlacedResourceAllocatorBase::Allocate(const UINT64&                a_allocationSize,
													const UINT64&                a_alignment,
													const Device&                a_device,
														  UINT64&                a_heapOffset,
														  TypeAlias::HeapPageID& a_heapPageID)
{
	a_heapPageID = Constant::k_invalidHeapPageID;
	a_heapOffset = k_initialOffset;

	if (a_allocationSize == 0ULL)
	{
		assert(false && "確保サイズが0のため、PlacedResourceAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	if (a_alignment == 0ULL)
	{
		assert(false && "アライメントが0のため、PlacedResourceAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	if (m_heapPageSize == 0ULL)
	{
		assert(false && "既定のヒープページサイズが未設定のため、PlacedResourceAllocatorの領域確保処理に失敗しました。");
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
		assert(false && "新規HeapPageの作成に失敗したため、PlacedResourceAllocatorの領域確保処理に失敗しました。");
		return false;
	}

	if (m_heapPageList.empty())
	{
		assert(false && "HeapPage追加後にページリストが空のため、PlacedResourceAllocatorの領域確保処理に失敗しました。");
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

bool FWK::Graphics::PlacedResourceAllocatorBase::Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize, TypeAlias::HeapPageID a_heapPageID)
{
	if (a_heapPageID == Constant::k_invalidHeapPageID)
	{
		assert(false && "無効なHeapPageIDが指定されたため、PlacedResourceAllocatorの領域解放処理に失敗しました。");
		return false;
	}

	if (a_allocationSize == 0ULL)
	{
		assert(false && "解放対象のサイズが0のため、PlacedResourceAllocatorの領域解放処理に失敗しました。");
		return false;
	}

	auto* const l_heapPage = FindMutablePTRHeapPage(a_heapPageID);

	if (!l_heapPage)
	{
		assert(false && "解放対象のHeapPageが見つからないため、PlacedResourceAllocatorの領域解放処理に失敗しました。");
		return false;
	}

	if (!l_heapPage->Release(a_heapOffset, a_allocationSize))
	{
		assert(false && "HeapPageの領域解放処理に失敗しました");
		return false;
	}

	return true;
}

const FWK::Graphics::HeapPage* FWK::Graphics::PlacedResourceAllocatorBase::FindPTRHeapPage(const TypeAlias::HeapPageID a_heapPageID) const
{
	const auto& l_itr = std::find_if(m_heapPageList.begin(), m_heapPageList.end(), [a_heapPageID](const HeapPage& a_heapPage) 
	{
		return a_heapPage.GetVALHeapPageID() == a_heapPageID;
	});

	if (l_itr == m_heapPageList.end()) { return nullptr; }

	return std::to_address(l_itr);
}

FWK::Graphics::HeapPage* FWK::Graphics::PlacedResourceAllocatorBase::FindMutablePTRHeapPage(const TypeAlias::HeapPageID a_heapPageID)
{
	const auto& l_itr = std::find_if(m_heapPageList.begin(), m_heapPageList.end(), [a_heapPageID](const HeapPage& a_heapPage) 
	{
		return a_heapPage.GetVALHeapPageID() == a_heapPageID;
	});

	if (l_itr == m_heapPageList.end()) { return nullptr; }

	return std::to_address(l_itr);
}

bool FWK::Graphics::PlacedResourceAllocatorBase::CreateHeapPage(const Device& a_device, const UINT64& a_allocationSize, const UINT64& a_alignment)
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

FWK::TypeAlias::HeapPageID FWK::Graphics::PlacedResourceAllocatorBase::GenerateHeapPageID()
{
	const TypeAlias::HeapPageID l_heapPageID = m_nextHeapPageID;

	if (l_heapPageID == Constant::k_invalidHeapPageID)
	{
		assert(false && "HeapPageID採番中にオーバーフローが発生しました。");
		return Constant::k_invalidHeapPageID;
	}

	// 次回のヒープページID用に更新
	++m_nextHeapPageID;

	return l_heapPageID;
}