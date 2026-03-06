#include "TextureContext.h"

void FWK::Graphics::TextureContext::Init()
{
	m_nextID = k_initialID;

	m_pathToIDMap.clear           ();
	m_recordList.clear            ();
	m_textureIDToSRVIndexMap.clear();
	m_pendingUploadList.clear     ();
	m_uploadPageList.clear        ();

	const TextureLoaderFunc& l_wicFileLoader = [](const std::wstring& a_filePath, DirectX::TexMetadata& a_texMetadata, DirectX::ScratchImage& a_scratchImage) 
	{
		return DirectX::LoadFromWICFile(a_filePath.c_str(),
										DirectX::WIC_FLAGS_FORCE_RGB,
										&a_texMetadata,
										a_scratchImage);
	};

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionPNG,  l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionJPG,  l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionJPEG, l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionBMP,  l_wicFileLoader);

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionDDS, [](const std::wstring& a_filePath, DirectX::TexMetadata& a_texMetadata, DirectX::ScratchImage& a_scratchImage)
	{
			return DirectX::LoadFromDDSFile(a_filePath.c_str(),
											DirectX::DDS_FLAGS_NONE,
											&a_texMetadata,
											a_scratchImage);
	});

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionTGA, [](const std::wstring& a_filePath, DirectX::TexMetadata& a_texMetadata, DirectX::ScratchImage& a_scratchImage)
	{
			return DirectX::LoadFromTGAFile(a_filePath.c_str(),
											&a_texMetadata,
											a_scratchImage);
	});
}

FWK::Graphics::Texture FWK::Graphics::TextureContext::LoadFile(const std::string& a_filePath)
{
	// 拡張子を取得
	auto l_extension = std::filesystem::path(a_filePath).extension().string();

	// すべての文字を小文字に変換
	std::transform(l_extension.begin(),
				   l_extension.end  (),
				   l_extension.begin(),
				   [](unsigned char a_char) 
					{
						return static_cast<char>(std::tolower(a_char));
					});

	auto l_itr = m_textureLoaderMap.find(l_extension);

	if (l_itr == m_textureLoaderMap.end())
	{
		assert(false && "未対応のテクスチャフォーマットです。");
		return Texture();
	}

	return LoadTexture(a_filePath, l_itr->second);
}

void FWK::Graphics::TextureContext::FlushUploads()
{
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

	// GPU完了待ち + allocator再利用
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

			// GetCopyableFootprintsのOffsetは「UploadResource先頭基準」なのでページ内オフセットを加算
			l_src.PlacedFootprint.Offset += l_pending.uploadBaseOffset;

			l_copyCommandList.CopyTextureRegion(l_dest, l_src);
		}

		const auto l_srvState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		// Executeは一回
		l_copyCommandList.TransitionResource(l_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, l_srvState);
	}

	// Signalは一回(このバッチ処理完了のフェンス値)
	l_copyCommandQueue.ExecuteCommandLists(l_copyCommandList);

	l_copyCommandQueue.SignalAndTracAllocator(l_copyCommandAllocator);

	const UINT64 l_fenceValue = l_copyCommandAllocator.GetSubmittedFenceValue();

	for (auto& l_pending : m_pendingUploadList)
	{
		if (!l_pending.uploadPage)
		{
			continue;
		}

		l_pending.record.currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
										D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		l_pending.uploadPage->fenceValue = std::max(l_pending.uploadPage->fenceValue, l_fenceValue);


		m_textureIDToSRVIndexMap[l_pending.record.textureID] = l_pending.record.srvIndex;
		m_recordList.emplace_back(std::move(l_pending.record));
	}

	m_pendingUploadList.clear();
}

UINT FWK::Graphics::TextureContext::FetchSRVIndex(const TextureID a_id) const
{
	// TextureID -> SRVIndexの高速検索
	if (auto l_itr = m_textureIDToSRVIndexMap.find(a_id);
		l_itr != m_textureIDToSRVIndexMap.end())
	{
		return l_itr->second;
	}

	assert(false && "指定テクスチャIDに対応するSRVインデックスが見つかりませんが見つかりません。");
	return k_invalideSRVIndex;
}

FWK::Graphics::Texture FWK::Graphics::TextureContext::LoadTexture(const std::string& a_filePath, const TextureLoaderFunc& a_textureLoaderFunc)
{
	// キャッシュ確認
	if (auto l_itr = m_pathToIDMap.find(a_filePath);
		l_itr != m_pathToIDMap.end())
	{
		return Texture(l_itr->second);
	}

	// DirectXTexで読み込み
	auto l_texMetadata  = DirectX::TexMetadata ();
	auto l_scratchImage = DirectX::ScratchImage();

	// ワイド文字列に変換
	const auto l_filePath = Utility::String::StringToWString(a_filePath);

	const auto l_hr = a_textureLoaderFunc(l_filePath, l_texMetadata, l_scratchImage);

	if (FAILED(l_hr))
	{
		assert(false && "DirectXTexでの画像読み込みに失敗しました。パスや対応形式を確認してください");
		return Texture();
	}

	// GPUリソース作成 & 転送
	auto l_record = CommonStruct::TextureRecord();

	l_record.textureID  = m_nextID++;
	l_record.sourcePath = a_filePath;

	if (!CreateTextureResourceAndUpload(l_texMetadata, l_scratchImage, l_record))
	{
		assert(false && "テクスチャのGPUアップロードに失敗しました。");
		return Texture();
	}

	// 登録
	m_pathToIDMap[a_filePath] = l_record.textureID;

	return Texture(l_record.textureID);
}

bool FWK::Graphics::TextureContext::CreateTextureResourceAndUpload(const DirectX::TexMetadata& a_metadata, const DirectX::ScratchImage& a_image, CommonStruct::TextureRecord& a_outRecord)
{
	auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	auto& l_resourceContext = l_graphicsManager.GetResourceContext();

	auto&       l_srvDescriptorAllocator = l_resourceContext.GetWorkSRVDescriptorAllocator();
	const auto& l_srvDescriptorHeap      = l_resourceContext.GetDescriptorHeapContext     ().GetSRVDescriptorHeap();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	const auto& l_desc = CD3DX12_RESOURCE_DESC::Tex2D(a_metadata.format,
													  a_metadata.width,
													  static_cast<UINT>  (a_metadata.height),
													  static_cast<UINT16>(a_metadata.arraySize),
													  static_cast<UINT16>(a_metadata.mipLevels));

	ComPtr<ID3D12Heap>      l_texHeap     = nullptr;
	ComPtr<ID3D12Resource2> l_texResource = nullptr;

	if (!AllocateDefaultHeapTexture(l_desc, l_texHeap, l_texResource))
	{
		assert(false && "CreateHeap + CreatePlacedResourceに失敗しました。");
		return false;
	}

	a_outRecord.currentState = D3D12_RESOURCE_STATE_COMMON;

	// UploadBufferの作成
	// GetCopyableFootpringsこのテクスチャをUploadBuffer煮詰めるときのサブリソースごとのレイアウト、
	// row count / row size全体の必要サイズを返す。CopyTextureRegionで必要な情報
	const auto l_subResourceCount = static_cast<UINT>(a_metadata.mipLevels * a_metadata.arraySize);

	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> l_layoutList  (l_subResourceCount);
	std::vector<UINT>								l_rowCountList(l_subResourceCount);
	std::vector<UINT64>								l_rowSizeList (l_subResourceCount);

	UINT64 l_requiredUploadSize = 0ULL;

	l_device->GetCopyableFootprints(&l_desc,
									0U,
									l_subResourceCount,
									0ULL,
									l_layoutList.data  (),
									l_rowCountList.data(),
									l_rowSizeList.data (),
									&l_requiredUploadSize);

	const UINT64 l_alignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;

	auto l_uploadAllocation = UploadAllocation();

	if (!AllocateUploadMemory(l_requiredUploadSize, l_alignment, l_uploadAllocation))
	{
		assert(false && "UploadArenaの確保に失敗。");
		return false;
	}

	UploadPage* const l_uploadPage = l_uploadAllocation.page;
	const UINT64      l_offset     = l_uploadAllocation.offset;

	if (!l_uploadPage)
	{
		assert(false && "l_uploadPageがnullptrでした。");
		return false;
	}

	// ※ 重要 : 各サブリソースのPlacedFootPring.Offsetは「UploadResource先頭からのOffset」なので
	// ページ内おfセットを足して使う必要がある
	for (UINT l_sub = 0U; l_sub < l_subResourceCount; ++l_sub)
	{
		const DirectX::Image* l_srcImage = a_image.GetImage(l_sub, 0ULL, 0ULL);

		if (!l_srcImage)
		{
			assert(false && "ScratchImageからサブリソースが取れない。");
			return false;
		}

		const auto&   l_layout  = l_layoutList  [l_sub];
		const UINT    l_rows    = l_rowCountList[l_sub];
		const UINT64& l_rowSize = l_rowSizeList [l_sub];

		// destは「ページmapped先頭 + ページ割り当てオフセット + サブリソースオフセット」
		std::uint8_t*       l_descBase = l_uploadPage->mappedPtr + l_offset + l_layout.Offset;
		const std::uint8_t* l_srcBase  = l_srcImage->pixels;

		// RowPitchが一致しないことがあるので行単位コピー
		for (UINT l_row = 0U; l_row < l_rows; ++l_row)
		{
			std::memcpy(l_descBase + (static_cast<std::size_t>(l_row) * l_layout.Footprint.RowPitch),
						l_srcBase +  (static_cast<std::size_t>(l_row) * l_srcImage->rowPitch),
						static_cast<std::size_t>(l_rowSize));
		}
	}

	// SRV確保(SRVはGPUコピー完了前に作っOK : 参照するのは描画時だが使う前にFlushが必要)
	const UINT l_srcIndex = l_srvDescriptorAllocator.Allocate();

	a_outRecord.resource = l_texResource;
	a_outRecord.heap     = l_texHeap;
	a_outRecord.srvIndex = l_srcIndex;

	auto l_srvDesc = D3D12_SHADER_RESOURCE_VIEW_DESC();

	l_srvDesc.Format                        = l_desc.Format;
	l_srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
	l_srvDesc.Texture2D.MipLevels           = l_desc.MipLevels;
	l_srvDesc.Texture2D.MostDetailedMip     = 0;
	l_srvDesc.Texture2D.ResourceMinLODClamp = 0.0F;
	l_srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	const auto& l_cpuHandle = l_srvDescriptorHeap.FetchCPUHandle(l_srcIndex);

	l_device->CreateShaderResourceView(l_texResource.Get(), &l_srvDesc, l_cpuHandle);

	auto l_pending = PendingUpload();

	l_pending.textureResource  = l_texResource;
	l_pending.uploadPage       = l_uploadPage;
	l_pending.uploadBaseOffset = l_offset;
	l_pending.subResourceCount = l_subResourceCount;
	l_pending.layoutlist       = std::move(l_layoutList);
	l_pending.rowCountList     = std::move(l_rowCountList);
	l_pending.rowSizeList      = std::move(l_rowSizeList);
	l_pending.srvIndex         = l_srcIndex;

	l_pending.record = a_outRecord;

	m_pendingUploadList.emplace_back(std::move(l_pending));

	return true;
}

bool FWK::Graphics::TextureContext::AllocateDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc, ComPtr<ID3D12Heap>& a_outHeap, ComPtr<ID3D12Resource2>& a_outResource)
{
	const auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	// 指定のResourceDescをDEFAULTなどのヒープに置くときに必要なサイズとアライメントを返す
	const auto& l_allocationInfo = l_device->GetResourceAllocationInfo(0U, 1U, &a_desc);

	// 画像用DEFAULTヒープ
	auto l_heapDesc = D3D12_HEAP_DESC();

	l_heapDesc.SizeInBytes = l_allocationInfo.SizeInBytes;
	l_heapDesc.Alignment   = l_allocationInfo.Alignment;
	l_heapDesc.Properties  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	l_heapDesc.Flags       = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

	auto l_hr = l_device->CreateHeap(&l_heapDesc, IID_PPV_ARGS(a_outHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "CreateHeapに失敗しました。");
		return false;
	}

	// CreatePlacedResourceはヒープの中のoffset位置にResourceを配置して作るAPI。
	// offsetは通常、allocationInfo.Alignmentに合わせる必要がある
	const UINT64 l_offset = 0ULL;

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

bool FWK::Graphics::TextureContext::AllocateUploadMemory(const UINT64& a_size, const UINT64& a_alignment, UploadAllocation& a_outAllocation)
{
	GarbageCollectUploadPage();

	const auto& l_alignedSize = Utility::Math::AlignUp(a_size, a_alignment);

	// 既存ページから探す
	for (auto& l_page : m_uploadPageList)
	{
		// GPU使用中のページは触らない
		if (l_page.fenceValue != 0ULL)
		{
			continue;
		}

		const UINT64 l_alinedOffset = Utility::Math::AlignUp(l_page.offset, a_alignment);

		if (l_alinedOffset + l_alignedSize <= l_page.size)
		{
			a_outAllocation.page   = &l_page;
			a_outAllocation.offset = l_alinedOffset;
			l_page.offset = l_alinedOffset + l_alignedSize;
			return true;
		}
	}

	// 足りないので新規ページ作成
	const UINT64 l_defaultPageSize = 64ULL * 1024ULL * 1024ULL;
	const UINT64 l_newPageSize = (l_defaultPageSize > l_alignedSize) ? l_defaultPageSize : Utility::Math::AlignUp(l_alignedSize, 1024ULL * 1024ULL);

	auto l_newPage = UploadPage();

	if (!CreateUploadPage(l_newPageSize, l_newPage))
	{
		return false;
	}

	m_uploadPageList.emplace_back(std::move(l_newPage));

	a_outAllocation.page         = &m_uploadPageList.back();
	a_outAllocation.offset       = 0ULL;
	a_outAllocation.page->offset = l_alignedSize;

	return true;
}

void FWK::Graphics::TextureContext::GarbageCollectUploadPage()
{
	auto&       l_graphicsManager = GraphicsManager::GetInstance        ();
	const auto& l_resourceContext = l_graphicsManager.GetResourceContext();
	
	const UINT64& l_completed = l_resourceContext.GetCopyCommandQueue().GetFence().GetCompletedFenceValue();

	for (auto& l_page : m_uploadPageList)
	{
		if (l_page.fenceValue == 0ULL)
		{
			continue;
		}

		if (l_completed >= l_page.fenceValue)
		{
			l_page.offset     = 0ULL;
			l_page.fenceValue = 0ULL;
		}
	}
}

bool FWK::Graphics::TextureContext::CreateUploadPage(const UINT64 a_pageSize, UploadPage& a_outPage) const
{
	const auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	const auto& l_desc     = CD3DX12_RESOURCE_DESC::Buffer(a_pageSize);
	const auto& l_heapProp = CD3DX12_HEAP_PROPERTIES      (D3D12_HEAP_TYPE_UPLOAD);

	// CreateComittedResourceはヒープとリソースをまとめて作る
	auto l_hr = l_device->CreateCommittedResource(&l_heapProp,
												  D3D12_HEAP_FLAG_NONE,
												  &l_desc,
												  D3D12_RESOURCE_STATE_GENERIC_READ,
												  nullptr,
												  IID_PPV_ARGS(a_outPage.resource.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "UploadPageのCreateComittedResourceに失敗。");
		return false;
	}

	void* l_mapped = nullptr;

	const auto& l_readRange = CD3DX12_RANGE(0ULL, 0ULL); // CPU readしないので0,0

	l_hr = a_outPage.resource->Map(0U, &l_readRange, &l_mapped);

	if (FAILED(l_hr))
	{
		assert(false && "UploadPageのMapに失敗。");
		return false;
	}

	a_outPage.mappedPtr  = reinterpret_cast<std::uint8_t*>(l_mapped);
	a_outPage.size       = a_pageSize;
	a_outPage.offset     = 0ULL;
	a_outPage.fenceValue = 0ULL;

	return true;
}