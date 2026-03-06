#include "TextureUploader.h"

void FWK::Graphics::TextureUploader::Init()
{
	m_pendingUploadList.clear();
	m_uploadPageList.clear   ();
}

bool FWK::Graphics::TextureUploader::StageTexture(const DirectX::TexMetadata& a_texMetadata, const DirectX::ScratchImage& a_image, CommonStruct::TextureRecord& a_inOutRecord)
{
	// デコード済み画像をGPU転送待ち状態まで準備する
	auto&		l_graphicsManager = GraphicsManager::GetInstance ();
	const auto& l_device		  = l_graphicsManager.GetHardware().GetDevice().GetDevice();
	
	auto& l_resourceContext = l_graphicsManager.GetResourceContext();

	auto&       l_srvDescriptorAllocator = l_resourceContext.GetWorkSRVDescriptorAllocator();
	const auto& l_srvDescriptorHeap      = l_resourceContext.GetDescriptorHeapContext     ().GetSRVDescriptorHeap();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	const auto& l_desc = CD3DX12_RESOURCE_DESC::Tex2D(a_texMetadata.format,
													  a_texMetadata.width,
													  static_cast<UINT>  (a_texMetadata.height),
													  static_cast<UINT16>(a_texMetadata.arraySize),
													  static_cast<UINT16>(a_texMetadata.mipLevels));

	ComPtr<ID3D12Heap>      l_texHeap     = nullptr;
	ComPtr<ID3D12Resource2> l_texResource = nullptr;

	if (!AllocateDefaultHeapTexture(l_desc, l_texHeap, l_texResource))
	{
		assert(false && "CreateHeap + CreatePlacedResourceに失敗しました。");
		return false;
	}

	// 作成直前の空のGPUテクスチャはCOMMON状態として扱う
	// 後でFlushUploads()の中でCOPY_DESC => SHADER_RESOURCEへ遷移する。
	a_inOutRecord.currentState = D3D12_RESOURCE_STATE_COMMON;
	a_inOutRecord.resource     = l_texResource;
	a_inOutRecord.heap         = l_texHeap;
	a_inOutRecord.format       = a_texMetadata.format;
	a_inOutRecord.width        = static_cast<UINT>(a_texMetadata.width);
	a_inOutRecord.height       = static_cast<UINT>(a_texMetadata.height);

	// UploadBufferの作成
	// GetCopyableFootpringsでこのテクスチャをUploadBuffer煮詰めるときのサブリソースごとのレイアウト、
	// row count / row size全体の必要サイズを返す。CopyTextureRegionで必要な情報
	// サブリソース数 = mip数 * arrayサイズ
	const auto l_subResourceCount = static_cast<UINT>(a_texMetadata.mipLevels * a_texMetadata.arraySize);

	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> l_layoutList  (l_subResourceCount); // 各サブリソースをUploadBUfferのどこに置くかの情報
	std::vector<UINT>								l_rowCountList(l_subResourceCount); // そのサブリソースが何行目にあるか
	std::vector<UINT64>								l_rowSizeList (l_subResourceCount); // 各行の実データサイズ

	UINT64 l_requiredUploadSize = 0ULL;

	// UploadBUfferへ詰めるときの各サブリソースの配置情報を取得する。
	// CopyTextureRegionで必要になる重要情報
	l_device->GetCopyableFootprints(&l_desc,
									0U,
									l_subResourceCount,
									0ULL,
									l_layoutList.data  (),
									l_rowCountList.data(),
									l_rowSizeList.data (),
									&l_requiredUploadSize);

	const UINT64 l_alignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;

	auto l_uploadAllocation = CommonStruct::UploadAllocation();

	if (!AllocateUploadMemory(l_requiredUploadSize, l_alignment, l_uploadAllocation))
	{
		assert(false && "UploadArenaの確保に失敗。");
		return false;
	}

	CommonStruct::UploadPage* const l_uploadPage = l_uploadAllocation.page;
	const UINT64			        l_offset     = l_uploadAllocation.offset;

	if (!l_uploadPage)
	{
		assert(false && "l_uploadPageがnullptrでした。");
		return false;
	}

	// ScratchImageの各サブリソースをUploadPageへコピーする
	// RowPitchが一致しないことがあるため行単位コピーで安全に詰める
	for (UINT l_sub = 0U; l_sub < l_subResourceCount; ++l_sub)
	{
		const DirectX::Image* l_srcImage = a_image.GetImage(l_sub, 0ULL, 0ULL);

		if (!l_srcImage)
		{
			assert(false && "ScratchImageからサブリソースが取れない。");
			return false;
		}

		// サブリソース分各配列を作成しているので要素数チェックを行う必要性がない
		const auto&   l_layout  = l_layoutList  [l_sub];
		const UINT    l_rows    = l_rowCountList[l_sub];
		const UINT64& l_rowSize = l_rowSizeList [l_sub];

		// GetCopyableFootpringsのoffsetはUploadResource先頭基準なので、
		// 実際の書き込み先は「ページ先頭 + この割り当てのオフセット + サブリソースオフセット」になる。
		std::uint8_t*       l_destBase = l_uploadPage->mappedPtr + l_offset + l_layout.Offset;
		const std::uint8_t* l_srcBase  = l_srcImage->pixels;

		// RowPitchが一致しないことがあるので行単位コピー
		for (UINT l_row = 0U; l_row < l_rows; ++l_row)
		{
			// 行単位でmemcpyを行う
			std::memcpy(l_destBase + (static_cast<std::size_t>(l_row) * l_layout.Footprint.RowPitch),
						l_srcBase  + (static_cast<std::size_t>(l_row) * l_srcImage->rowPitch),
						l_rowSize);
		}
	}

	// SRVはGPUコピー完了前に作成してよい。
	// 実際に参照されるのは描画時であり、その時点までにFlushUploads()が終わっていれば問題ない
	const UINT l_srvIndex = l_srvDescriptorAllocator.Allocate();

	// SRVDescriptorHeapにディスクリプタとして格納するためのインデックスを取得
	a_inOutRecord.srvIndex = l_srvIndex;

	// l_textureResourceをシェーダーからどう見せるかを設定
	auto l_srvDesc = D3D12_SHADER_RESOURCE_VIEW_DESC();

	l_srvDesc.Format                        = l_desc.Format;
	l_srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D; // 現状は2Dテクスチャ
	l_srvDesc.Texture2D.MipLevels           = l_desc.MipLevels;
	l_srvDesc.Texture2D.MostDetailedMip     = 0;
	l_srvDesc.Texture2D.ResourceMinLODClamp = 0.0F;
	l_srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// SRVヒープの書き込み先を取得
	const auto& l_cpuHandle = l_srvDescriptorHeap.FetchCPUHandle(l_srvIndex);

	// SRVDescriptorHeapに作成
	l_device->CreateShaderResourceView(l_texResource.Get(), &l_srvDesc, l_cpuHandle);

	// FlushUploads()で消費するpendingデータを積む
	auto l_pending = CommonStruct::PendingUpload();

	l_pending.textureResource  = l_texResource;
	l_pending.uploadPage       = l_uploadPage;
	l_pending.subResourceCount = l_subResourceCount;
	l_pending.layoutlist       = std::move(l_layoutList);
	l_pending.rowCountList     = std::move(l_rowCountList);
	l_pending.rowSizeList      = std::move(l_rowSizeList);
	l_pending.srvIndex         = l_srvIndex;
	l_pending.uploadBaseOffset = l_offset;
	l_pending.record           = a_inOutRecord;

	// バッチ処理を行うためにリストに情報を格納
	m_pendingUploadList.emplace_back(std::move(l_pending));

	return true;
}

void FWK::Graphics::TextureUploader::FlushUploads(std::vector<CommonStruct::TextureRecord>& a_outUploadedRecordList)
{
	// StatgeTexture()で積んだPendingUploadをまとめてGPUへ送る。
	// ExecuteCommandListsとSignalはバッチ残体で一回ずつだけ行う。
	if (m_pendingUploadList.empty())
	{
		return;
	}

	auto& l_graphicsManager = GraphicsManager::GetInstance        ();
	auto& l_resourceContext = l_graphicsManager.GetResourceContext();
	
	auto&		l_copyCommandQueue     = l_resourceContext.GetWorkCopyCommandQueue    ();
	auto&		l_copyCommandAllocator = l_resourceContext.GetWorkCopyCommandAllocator();
	const auto& l_copyCommandList      = l_resourceContext.GetCopyCommandList         ();

	GarbageCollectUploadPage();

	// GPU完了待ちを必要があれば行い、安全にallocatorを再利用する
	l_copyCommandQueue.EnsureAllocatorAvailable(l_copyCommandAllocator);

	l_copyCommandAllocator.Reset();
	l_copyCommandList.Reset     (l_copyCommandAllocator);

	for (auto& l_pending : m_pendingUploadList)
	{
		const auto& l_texture    = l_pending.textureResource;
		const auto& l_uploadPage = l_pending.uploadPage;

		if (!l_texture)
		{
			assert(false && "テクスチャが生成されていません。");
			return;
		}

		if (!l_uploadPage || !l_uploadPage->resource)
		{
			assert(false && "UploadPageが不正です。");
			return;
		}

		l_copyCommandList.TransitionResource(l_texture.Get(), l_pending.record.currentState, D3D12_RESOURCE_STATE_COPY_DEST);

		for (UINT l_sub = 0U; l_sub < l_pending.subResourceCount; ++l_sub)
		{
			auto l_dest = D3D12_TEXTURE_COPY_LOCATION();
			
			l_dest.pResource        = l_texture.Get();
			l_dest.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			l_dest.SubresourceIndex = l_sub;

			auto l_src = D3D12_TEXTURE_COPY_LOCATION();

			l_src.pResource       = l_uploadPage->resource.Get();
			l_src.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			l_src.PlacedFootprint = l_pending.layoutlist[l_sub];

			// GetCopyableFootpringsのOffsetはUploadResource戦闘基準
			// 1ページ内のどの子に積んだかを反映するため、uploadBaseOffsetを加算する。
			l_src.PlacedFootprint.Offset += l_pending.uploadBaseOffset;

			l_copyCommandList.CopyTextureRegion(l_dest, l_src);
		}

		const auto l_shaderResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
										   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		// Executeは一回
		l_copyCommandList.TransitionResource(l_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, l_shaderResourceState);
	}

	// バッチ全体を一回だけ実行し、完了フェンスも一回だけ発行する。
	l_copyCommandQueue.ExecuteCommandLists   (l_copyCommandList);
	l_copyCommandQueue.SignalAndTracAllocator(l_copyCommandAllocator);

	const UINT64 l_fenceValue = l_copyCommandAllocator.GetSubmittedFenceValue();

	// このバッチに含まれていたrecordを関数呼びだし元へ返す
	// 呼びだし元はこれを使ってregistryへ登録する
	for (auto& l_pending : m_pendingUploadList)
	{
		if (!l_pending.uploadPage)
		{
			continue;
		}

		l_pending.record.currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
										D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		l_pending.uploadPage->fenceValue = std::max(l_pending.uploadPage->fenceValue, l_fenceValue);

		a_outUploadedRecordList.emplace_back(std::move(l_pending.record));
	}

	// 一括でGPUに送信する役目を終えたのでクリア
	m_pendingUploadList.clear();
}

bool FWK::Graphics::TextureUploader::AllocateDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc, ComPtr<ID3D12Heap>& a_outHeap, ComPtr<ID3D12Resource2>& a_outResource) const
{
	// DEFAULTヒープ上にCreateHeap + CreatePlacedResourceでGPUテクスチャを作る
	const auto& l_graphicsManager = GraphicsManager::GetInstance ();
	const auto& l_deviceWrapper   = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device          = l_deviceWrapper.GetDevice    ();
	
	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	// DEFAULTヒープに置くときに必要なサイズとアライメントを取得
	const auto& l_allocationInfo = l_device->GetResourceAllocationInfo(l_deviceWrapper.GetGPUNodeMask(), k_sendResourceDescNum, &a_desc);

	// 画像用DEFAULTヒープ
	auto l_heapDesc = D3D12_HEAP_DESC();

	l_heapDesc.SizeInBytes = l_allocationInfo.SizeInBytes;
	l_heapDesc.Alignment   = l_allocationInfo.Alignment;
	l_heapDesc.Properties  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	l_heapDesc.Flags       = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;    // RenderTargetとDepthStencil意外のテクスチャだけ置いて良いという制限フラグ

	// GPU用のヒープ領域を確保
	auto l_hr = l_device->CreateHeap(&l_heapDesc, IID_PPV_ARGS(a_outHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "CreateHeapに失敗しました。");
		return false;
	}

	// CreatePlacedResourceはヒープの中のoffset位置にResourceを配置して作るAPI。
	// offsetは通常、allocationInfo.Alignmentに合わせる必要がある
	const UINT64 l_offset = 0ULL;

	// 確保したメモリ領域をテクスチャとして扱うことを宣言する
	l_hr = l_device->CreatePlacedResource(a_outHeap.Get(),
										  l_offset, 
										  &a_desc, 
										  D3D12_RESOURCE_STATE_COMMON, // まずはCommonで作成し、後でCopyDescへ遷移
										  nullptr,
										  IID_PPV_ARGS(a_outResource.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "CreatePlacedResourceに失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::TextureUploader::AllocateUploadMemory(const UINT64& a_size, const UINT64& a_alignment, CommonStruct::UploadAllocation& a_outAllocation)
{
	// UploadPageArenaから必要サイズ分の領域を確保する。
	// 既存ページに空きがあればそこを使い、足りなければ新規ページを作る。
	GarbageCollectUploadPage();

	const auto& l_alignedSize = Utility::Math::AlignUp(a_size, a_alignment);

	// 既存ページから探す
	for (auto& l_page : m_uploadPageList)
	{
		// 未使用のアップロードページのみ使用
		if (l_page.fenceValue != CommonConstant::k_unusedFenceValue)
		{
			continue;
		}

		const UINT64& l_alinedOffset = Utility::Math::AlignUp(l_page.offset, a_alignment);

		// サイズとオフセットをa_alignment倍数にして加算したものと比較し
		// ページサイズが足りていればオフセットとページサイズをa_outAllocationに渡し
		// 現在のページのオフセットを進める
		if (l_alinedOffset + l_alignedSize <= l_page.size)
		{
			a_outAllocation.page   = &l_page;
			a_outAllocation.offset = l_alinedOffset;

			l_page.offset = l_alinedOffset + l_alignedSize;
			return true;
		}
	}

	// 足りないので新規ページ作成
	const UINT64& l_defaultPageSize = k_defaultPageSize;
	const UINT64& l_newPageSize     = (l_defaultPageSize > l_alignedSize) ? l_defaultPageSize : Utility::Math::AlignUp(l_alignedSize, k_oneMegaByte);

	auto l_newPage = CommonStruct::UploadPage();

	if (!CreateUploadPage(l_newPageSize, l_newPage))
	{
		assert(false && "新規UplaodPageの作成に失敗しました。");
		return false;
	}

	// 新しいページをリストに格納
	m_uploadPageList.emplace_back(std::move(l_newPage));

	// a_outAllocation.pageにm_uploadPageList内で今作ったページのアドレスを渡す
	a_outAllocation.page         = &m_uploadPageList.back();
	a_outAllocation.offset       = 0ULL;
	a_outAllocation.page->offset = l_alignedSize;

	return true;
}

void FWK::Graphics::TextureUploader::GarbageCollectUploadPage()
{
	// GPUが使い終わったUploadPageを再利用可能状態へ戻す。
	// CompletedFence >= Page.fenceValueになったページはOffsetを0へ戻す。
	auto&       l_graphicsManager = GraphicsManager::GetInstance        ();
	const auto& l_resourceContext = l_graphicsManager.GetResourceContext();
	
	const UINT64& l_completed = l_resourceContext.GetCopyCommandQueue().GetFence().GetCompletedFenceValue();

	for (auto& l_page : m_uploadPageList)
	{
		// GPU使用中としてマークされていないので"return"
		if (l_page.fenceValue == CommonConstant::k_unusedFenceValue)
		{
			continue;
		}

		// ページが記録したフェンス値よりもGPUのフェンス値が高ければ実行
		if (l_completed >= l_page.fenceValue)
		{
			// このページの使用済み領域を全部リセットして次回は先頭から
			// また詰められるようにして再利用できるようにしている
			l_page.offset     = 0ULL;
			l_page.fenceValue = CommonConstant::k_unusedFenceValue;
		}
	}
}

bool FWK::Graphics::TextureUploader::CreateUploadPage(const UINT64& a_pageSize, CommonStruct::UploadPage& a_outPage) const
{
	// 新しいUploadPageを1枚作成する。
	// UPLOADヒープのバッファをCreateCommittedResourceで作り、Mapして保持する。
	const auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_device = l_graphicsManager.GetHardware().GetDevice().GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	// a_pageSizeバイトのバッファリソースを作成
	const auto& l_desc = CD3DX12_RESOURCE_DESC::Buffer(a_pageSize);

	// UPLOAD用のヒープ領域に割り当てる
	const auto& l_heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// CreateComittedResourceはヒープとリソースをまとめて作る
	auto l_hr = l_device->CreateCommittedResource(&l_heapProp,
												  D3D12_HEAP_FLAG_NONE,
												  &l_desc,
												  D3D12_RESOURCE_STATE_GENERIC_READ,						  // CPUが書き込めGPUがコピー元として読める状態で作成
												  nullptr,
												  IID_PPV_ARGS(a_outPage.resource.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "UploadPageのCreateComittedResourceに失敗。");
		return false;
	}

	void* l_mapped = nullptr;

	const auto& l_readRange = CD3DX12_RANGE(0ULL, 0ULL); // CPUは読まないことを明示

	l_hr = a_outPage.resource->Map(0U, &l_readRange, &l_mapped);

	if (FAILED(l_hr))
	{
		assert(false && "UploadPageのMapに失敗。");
		return false;
	}

	a_outPage.mappedPtr  = reinterpret_cast<std::uint8_t*>(l_mapped);
	a_outPage.size       = a_pageSize;
	a_outPage.offset     = 0ULL;
	a_outPage.fenceValue = CommonConstant::k_unusedFenceValue;

	return true;
}