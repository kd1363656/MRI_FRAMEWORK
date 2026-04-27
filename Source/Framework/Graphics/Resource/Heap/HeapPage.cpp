#include "HeapPage.h"

bool FWK::Graphics::HeapPage::Create(const UINT64&               a_heapSize,
                                     const D3D12_HEAP_TYPE       a_heapType,
                                     const D3D12_HEAP_FLAGS      a_heapFlags,
                                     const Device&               a_device,
                                     const TypeAlias::HeapPageID a_heapPageID)
{
    const auto& l_device = a_device.GetREFDevice();

    if (!l_device)
    {
        assert(false && "デバイスが作成されていないため、HeapPageの作成処理に失敗しました。");
        return false;
    }

    if (a_heapPageID == Constant::k_invalidHeapPageID)
    {
        assert(false && "無効なHeapPageIDが指定されたため、HeapPage作成処理に失敗しました。");
        return false;
    }

    if (a_heapSize == 0ULL)
    {
        assert(false && "ヒープサイズが0のため、HeapPage作成処理に失敗しました。");
        return false;
    }

    constexpr auto l_nodeMask = GraphicsManager::GetVALDefaultGPUNodeMask();

    // D3D12_HEAP_DESCについての説明
	// SizeInBytes : ヒープ全体のサイズ(何バイト確保するのか)
	// Properties  : ヒープのメモリ種別や所属ノードなどの性質
	// Alignment   : ヒープ全体のアライメント。0を指定すると基底値を使う
	// Flags       : このヒープに配置できるリソース種別の制限
    D3D12_HEAP_DESC l_heapDesc = {};

	// このページが持つヒープ総サイズ
	l_heapDesc.SizeInBytes = a_heapSize;

	// D3D12_HEAP_PROPERTIESについての説明
	// Type			        : このヒープをどういう用途のメモリとして作るか
	// CPUPageProperty      : CPUから見たページ属性
	// MemoryPoolPreference : このメモリをどのメモリプールに置きたいか
	// CreationNodeMask     : このヒープをどのGPUノードに所属するものとして作るか
	// VisibleNodeMask      : 作ったこのヒープをどのノードから見えるようにするか
	// a_heapTypeで指定された種類のヒープとして作成する
    l_heapDesc.Properties = CD3DX12_HEAP_PROPERTIES(a_heapType, l_nodeMask, l_nodeMask);

    // 0を指定すると既定アライメントを使う
	l_heapDesc.Alignment = 0ULL;

	// このHeapに配置できるリソース種別の制限を指定
	l_heapDesc.Flags = a_heapFlags;

	// CreateHeap(作成するヒープの設定
	//			  受け取りたいCOMインターフェース型のID、
	//			  作成結果のポインタを書き込むアドレス);

	const auto l_hr = l_device->CreateHeap(&l_heapDesc, IID_PPV_ARGS(m_heap.ReleaseAndGetAddressOf()));

    if (FAILED(l_hr))
    {
        assert(false && "D3D12Heapの作成処理に失敗しました。");
        return false;
    }

	m_heapSize   = a_heapSize;
	m_heapPageID = a_heapPageID;

    // ヒープ領域のサイズ、ヒープページIDを格納
	Struct::FreeHeapBlock l_initialFreeBlock = {};

	l_initialFreeBlock.beginOffset = Constant::k_unusedFreeHeapBlockBeginOffset;
	l_initialFreeBlock.size        = a_heapSize;

    m_freeHeapBlockList.emplace_back(l_initialFreeBlock);

    return true;
}

bool FWK::Graphics::HeapPage::Allocate(const UINT64& a_allocationSize, const UINT64& a_alignment, UINT64& a_heapOffset)
{
	a_heapOffset = 0ULL;

	if (!m_heap)
	{
		assert(false && "HeapPageが未作成のため、ヒープ領域確保処理に失敗しました。");
		return false;
	}

	if (a_allocationSize == 0ULL)
	{
		assert(false && "確保サイズが0のため、ヒープ領域確保処理に失敗しました。");
		return false;
	}

	if (a_alignment == 0ULL)
	{
		assert(false && "アライメントが0のため、ヒープ領域確保処理に失敗しました。");
		return false;
	}

	for (auto l_itr = m_freeHeapBlockList.begin(); l_itr != m_freeHeapBlockList.end(); ++l_itr)
	{
		// 今見ているFreeBlockの先頭オフセット
		const auto& l_blockBegin = l_itr->beginOffset;

		// 現在見ているFreeBlockの終端の「次」の位置。
		// 例 : Offset = 100、FreeSize = 300の場合、FreeBlockの開始は100
		// l_blockEndは400になる。
		const auto& l_blockEnd = l_itr->beginOffset + l_itr->size;

		// FreeBlockの開始位置をa_alignment境界に切り上げた開始オフセット
		// 例 : - l_blockBegin = 100
		//		- a_alignment  = 64
		// の場合、64の倍数で100以上の最初の値は128なので、
		// l_alignedOffset = 128になる。
		const auto& l_alignedOffset = Utility::Math::AlignUp(l_blockBegin, a_alignment);

		// 確保領域の終端の「次」の位置
		// 例 : - l_alignedOffset  = 128
		//		- a_allocationSize = 128
		// の場合、確保領域は128 - 255で、
		// l_allocationEndは256になる。
		const auto& l_allocationEnd = l_alignedOffset + a_allocationSize;

		// 確保領域の終端がFreeBlockの範囲を超えるなら、
		// この空きブロックに置けないので次のFreeBlockを調べる。
		if (l_allocationEnd > l_blockEnd)
		{
			continue;
		}

		// 確保領域の前に残るサイズ。
		// 例 : FreeBlock  = [100, 400)
		//      Allocation = [128, 256)
		// の場合、前に残る空きは[100, 128)なので28バイト
		const auto& l_prefixSize = l_alignedOffset - l_blockBegin;

		// 確保領域の後ろに残る空きサイズ。
		// 例 : FreeBlock  = [100, 400)
		//      Allocation = [128, 256)
		// の場合、後ろに残る空きは[256, 400)なので144バイト
		const auto& l_suffixSize = l_blockEnd - l_allocationEnd;

		// FreeBlock全体を丁度使い切るケース
		// 前にも後ろにも空きが残らないので、このFreeBlock自体を削除する
		if ((l_prefixSize == 0ULL) && (l_suffixSize == 0ULL))
		{
			m_freeHeapBlockList.erase(l_itr);
		}
		// 確保領域がFreeBlockの先頭から始まるケース。
		// 前側の空きは残らず、後ろ側の空きだけ残るので、
		// 現在のFreeBlockを「後ろ側の残り領域に更新する」
		else if (l_prefixSize == 0ULL)
		{
			l_itr->beginOffset = l_allocationEnd;
			l_itr->size	       = l_suffixSize;
		}
		// 確保領域がFreeBlockの終端ピッタリまで使うケース
		// 後ろ側の空きは残らず、前側の空きだけ残るので、
		// 現在のFreeBlockのサイズだけを前側の残りサイズに更新する
		else if (l_suffixSize == 0ULL)
		{
			l_itr->size = l_prefixSize;
		}
		// 確保領域がFreeBlockの途中に入るケース
		// そのためFreeBlockは前側の空きと後ろ側の空きの2つに分割される。
		// 現在のFreeBlockは前側として残し、後ろ側は新しいFreeBlockとして追加する
		else
		{
			auto l_suffixBlock = Struct::FreeHeapBlock();
			
			l_suffixBlock.beginOffset = l_allocationEnd;
			l_suffixBlock.size        = l_suffixSize;

			l_itr->size = l_prefixSize;

			m_freeHeapBlockList.insert(l_itr + 1ULL, l_suffixBlock);
		}

		a_heapOffset = l_alignedOffset;

		return true;
	}

    return false;
}

bool FWK::Graphics::HeapPage::Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize)
{
    if (!m_heap)
    {
        assert(false && "HeapPageが未作成のため、ヒープ領域解放処理に失敗しました。");
        return false;
    }

    if (a_allocationSize == 0ULL)
    {
        assert(false && "解放サイズが0のため、ヒープ領域解放処理に失敗しました。");
        return false;
    }

    // 解放された領域を新しいFreeBlockとしてFreeListに追加する。
	// ただし、この時点では前後のFreeBlockと連結できる可能性があるため、
	// 追加後にMergeFreeBlocks()を呼んで隣接、重複ブロックをまとめる
	if (a_heapOffset >= m_heapSize)
	{
		assert(false && "解放開始オフセットがヒープ範囲外のため、ヒープ領域解放処理に失敗しました。");
		return false;
	}

	if (a_allocationSize > (m_heapSize - a_heapOffset))
	{
		assert(false && "解放領域がヒープサイズを超過しているため、ヒープ領域解放処理に失敗しました。");
		return false;
	}

    if (HasOverlappingFreeBlock(a_heapOffset, a_allocationSize))
	{
		assert(false && "既存の空き領域と重複しており、二重解放または不正な解放の疑いがあるため、ヒープ領域解放処理に失敗しました。");
		return false;
	}

	auto l_releaseBlock = Struct::FreeHeapBlock();

	// 解放したヒープブロックをフリーのヒープブロックリストとして追加する
	l_releaseBlock.beginOffset = a_heapOffset;
	l_releaseBlock.size        = a_allocationSize;

	m_freeHeapBlockList.emplace_back(l_releaseBlock);

	MergeFreeBlocks();

	return true;
}

bool FWK::Graphics::HeapPage::HasOverlappingFreeBlock(const UINT64& a_beginOffset, const UINT64& a_freeSize) const
{
	if (a_freeSize == 0ULL)
	{
		assert(false && "比較対象の解放サイズが0のため、空き領域重複判定に失敗しました。");
		return true;
	}

	const auto& l_endOffset = a_beginOffset + a_freeSize;

	return std::ranges::any_of(m_freeHeapBlockList, [a_beginOffset, l_endOffset](const Struct::FreeHeapBlock& a_freeHeapBlock)
	{
		const auto& l_freeBlockBegin = a_freeHeapBlock.beginOffset;
		const auto& l_freeBlockEnd   = a_freeHeapBlock.beginOffset + a_freeHeapBlock.size;

		// [a_beginOffset, l_endOffset)と[l_freeBlockBegin, l_freeBlockEnd)が重なるか判定
		return (a_beginOffset < l_freeBlockEnd) && (l_freeBlockBegin < l_endOffset);
	});
}

void FWK::Graphics::HeapPage::MergeFreeBlocks()
{
	if (m_freeHeapBlockList.size() <= k_mergeStartIndex) { return; }

	// Offsetの小さい順に並べる(昇順)
	// これにより、「今見ているブロック」と「直前のブロック」が
	// 連結できるかどうかを順番に判定できるようになる。
	std::sort(m_freeHeapBlockList.begin(), m_freeHeapBlockList.end(), [](const Struct::FreeHeapBlock& a_first, const Struct::FreeHeapBlock& a_second) 
	{
		return a_first.beginOffset < a_second.beginOffset;
	});

	std::vector<Struct::FreeHeapBlock> l_mergedBlocks = {};

	// 念のため現在のフリーヒープブロックのサイズ分予約しておく
	// l_mergedBlockはヒープ領域のサイズを結合したものを格納する配列であるため
	// 現在のm_freeHeapBlockList.size()以内の要素は必ず確保するからである
	l_mergedBlocks.reserve(m_freeHeapBlockList.size());

	// ソートした後の先頭のフリーヒープブロックをl_mergedBlocksに追加
	l_mergedBlocks.emplace_back(m_freeHeapBlockList.front());

	// FreeListの中で、重複している部分や、隣接していて結合可能な部分を一つのFreeBlockにまとめる
	for (std::size_t l_index = k_mergeStartIndex; l_index < m_freeHeapBlockList.size(); ++l_index)
	{
		const auto& l_currentBlock = m_freeHeapBlockList[l_index];
			  auto& l_lastBlock    = l_mergedBlocks.back();

		// 直前に確定したブロックの終端の「次」の位置。
		// 例 : Offset = 100, FreeSize = 300なら実Rangeは100 - 399で、
		// l_lastBlockEndは400になる
		const UINT64& l_lastBlockEnd = l_lastBlock.beginOffset + l_lastBlock.size;

		// 現在のブロックの開始位置が、直前のブロックの終端以前なら
		// 2つのブロックは「隣接している」または「重なっている」。
		// そのため1つのブロックにまとめられる
		if (l_currentBlock.beginOffset <= l_lastBlockEnd)
		{
			const UINT64& l_currentBlockEnd = l_currentBlock.beginOffset + l_currentBlock.size;

			if (l_currentBlockEnd > l_lastBlockEnd)
			{
				l_lastBlock.size = l_currentBlockEnd - l_lastBlock.beginOffset;
			}
		}
		else
		{
			l_mergedBlocks.emplace_back(l_currentBlock);
		}
	}

	m_freeHeapBlockList = std::move(l_mergedBlocks);
}