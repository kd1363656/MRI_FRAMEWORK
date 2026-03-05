#include "TextureContext.h"

void FWK::Graphics::TextureContext::Init()
{
	m_nextID = k_initialID;

	m_pathToIDMap.clear();
	m_recordList.clear ();

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
	const auto l_extension = std::filesystem::path(a_filePath).extension().string();

	auto l_itr = m_textureLoaderMap.find(l_extension);

	if (l_itr == m_textureLoaderMap.end())
	{
		assert(false && "未対応のテクスチャフォーマットです。");
		return Texture();
	}

	return LoadTexture(a_filePath, l_itr->second);
}

UINT FWK::Graphics::TextureContext::FetchSRVIndex(const TextureID a_id) const
{
	// 線形探索
	// 最適化するならid->indexのunordered_mapを持つ
	for (const auto& l_record : m_recordList)
	{
		if (l_record.textureID == a_id)
		{
			return l_record.srvIndex;
		}
	}

	assert(false && "指定テクスチャIDが見つかりません。");
	return CommonConstant::k_invalidTextureID;
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

	if (!CreateTextureResourceAndUpload(a_filePath, l_texMetadata, l_scratchImage, l_record))
	{
		assert(false && "テクスチャのGPUアップロードに失敗しました。");
		return Texture();
	}

	// 登録
	m_pathToIDMap[a_filePath] = l_record.textureID;

	m_recordList.emplace_back(l_record);

	return Texture(l_record.textureID);
}

bool FWK::Graphics::TextureContext::CreateTextureResourceAndUpload(const std::string&           a_filePath, 
																   const DirectX::TexMetadata&  a_metadata,
																   const DirectX::ScratchImage& a_image,
																   CommonStruct::TextureRecord& a_outRecord)
{
	auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	auto& l_resourceContext = l_graphicsManager.GetResourceContext();

	auto&		l_copyCommandQueue     = l_resourceContext.GetWorkCopyCommandQueue    ();
	auto&		l_copyCommandAllocator = l_resourceContext.GetWorkCopyCommandAllocator();
	const auto& l_copyCommandList      = l_resourceContext.GetCopyCommandList         ();

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

	// UploadBufferの作成
	const UINT l_subResourceCount = static_cast<UINT>(a_metadata.mipLevels * a_metadata.arraySize);

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

	ComPtr<ID3D12Resource2> l_uploadBuffer;

	if (!CreateUploadBuffer(l_requiredUploadSize, l_uploadBuffer))
	{
		assert(false && "UploadeBufferの作成に失敗しました。");
		return false;
	}

	// UploadBufferへCPUコピー
	void* l_mapped = nullptr;

	const auto l_readRange = CD3DX12_RANGE(0ULL, 0ULL); // CPUが読むわけではないので0

	const auto l_hr = l_uploadBuffer->Map(0U, &l_readRange, &l_mapped);

	if (FAILED(l_hr))
	{
		assert(false && "UploadBufferのMapに失敗しました。");
		return false;
	}

	// ScratchImageはサブリソース単位で画像を持っている
	for (UINT l_sub = 0U; l_sub < l_subResourceCount; ++l_sub)
	{
		const DirectX::Image* l_srcImage = a_image.GetImage(l_sub, 0ULL, 0ULL);

		if (!l_srcImage)
		{
			assert(false && "ScratchImageからサブリソース画像が取得できません。");
			l_uploadBuffer->Unmap(0, nullptr);
			return false;
		}

		const auto&  l_layout  = l_layoutList  [l_sub];
		const UINT   l_rows    = l_rowCountList[l_sub];
		const UINT64 l_rowSize = l_rowSizeList [l_sub];

		std::uint8_t* l_dest = reinterpret_cast<std::uint8_t*>(l_mapped);

		const std::uint8_t* l_src = l_srcImage->pixels;

		// 行ごとにコピー(GPU側のRowPitchとソースのRowPitchが違うことがある)
		for (UINT l_row = 0U; l_row < l_rows; ++l_row)
		{
			std::memcpy(l_dest + (static_cast<std::size_t>(l_row) * l_layout.Footprint.RowPitch),
						l_src +  (static_cast<std::size_t>(l_row) * l_srcImage->rowPitch),
						static_cast<std::size_t>(l_rowSize));
		}
	}

	l_uploadBuffer->Unmap(0U, nullptr);

	// CopyTextureRegionでGPUへ転送

	// Copy用アロケータが前回のGPU実行を終えているかを確認(CPU/GPU並列の基本)
	l_copyCommandQueue.EnsureAllocatorAvailable(l_copyCommandAllocator);

	// Reset(このフレームのCopyコマンドを貯める)
	l_resourceContext.ResetCommandObjects();

	// 状態遷移(Common -> CopyDest)
	l_copyCommandList.TransitionResource(l_texResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

	// サブリソースごとにCopyTextureRegion
	for (UINT l_sub = 0U; l_sub < l_subResourceCount; ++l_sub)
	{
		auto l_dest = D3D12_TEXTURE_COPY_LOCATION();

		l_dest.pResource        = l_texResource.Get();
		l_dest.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		l_dest.SubresourceIndex = l_sub;

		auto l_src = D3D12_TEXTURE_COPY_LOCATION();

		l_src.pResource       = l_uploadBuffer.Get();
		l_src.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		l_src.PlacedFootprint = l_layoutList[l_sub];

		// 全領域コピー(必要なら領域指定も可能)
		l_copyCommandList.CopyTextureRegion(l_dest, l_src);
	}

	// 状態遷移(CopyDest -> PixelShaderResource)
	l_copyCommandList.TransitionResource(l_texResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// 実行
	l_copyCommandQueue.ExecuteCommandLists(l_copyCommandList);

	// allocator追跡(Fenceを進めて次の利用時に待てるようにする)
	l_copyCommandQueue.SignalAndTracAllocator(l_copyCommandAllocator);

	// SRV作成
	const UINT l_srvIndex = l_srvDescriptorAllocator.Allocate();

	D3D12_SHADER_RESOURCE_VIEW_DESC l_srvDesc = {};
	
	l_srvDesc.Format                  = a_metadata.format;
	l_srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	l_srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	l_srvDesc.Texture2D.MipLevels     = static_cast<UINT>(a_metadata.mipLevels);

	const auto l_cpuHandle = l_srvDescriptorHeap.FetchCPUHandle(l_srvIndex);

	l_device->CreateShaderResourceView(l_texResource.Get(), &l_srvDesc, l_cpuHandle);

	// 記録
	a_outRecord.resource = l_texResource;
	a_outRecord.srvIndex = l_srvIndex;
	a_outRecord.width    = static_cast<UINT>(a_metadata.width);
	a_outRecord.height   = static_cast<UINT>(a_metadata.height);
	a_outRecord.format   = a_metadata.format;

	return true;
}

bool FWK::Graphics::TextureContext::AllocateDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc, ComPtr<ID3D12Heap>& a_outHeap, ComPtr<ID3D12Resource2> a_outResource)
{
	const auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	// リソースが必要とするサイズ、アライメントを問い合わせ
	const auto& l_allocationInfo = l_device->GetResourceAllocationInfo(0U, 1U, &a_desc);

	// 画像用DEFAULTヒープ
	auto l_heapDesc = D3D12_HEAP_DESC();

	l_heapDesc.SizeInBytes = l_allocationInfo.SizeInBytes;
	l_heapDesc.Alignment   = l_allocationInfo.SizeInBytes;
	l_heapDesc.Properties  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	l_heapDesc.Flags       = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

	auto l_hr = l_device->CreateHeap(&l_heapDesc, IID_PPV_ARGS(a_outHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "CreateHeapに失敗しました。");
		return false;
	}

	// PlacedResourceは「ヒープの中のoffset位置に記録する」
	const UINT64 l_offet = 0ULL;

	l_hr = l_device->CreatePlacedResource(a_outHeap.Get(),
										  l_offet, 
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

bool FWK::Graphics::TextureContext::CreateUploadBuffer(const UINT64 a_size, ComPtr<ID3D12Resource2>& a_outUpload)
{
	const auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	const auto l_desc = CD3DX12_RESOURCE_DESC::Buffer(a_size);

	auto l_heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	const auto l_hr = l_device->CreateCommittedResource(&l_heapProp, 
													    D3D12_HEAP_FLAG_NONE,
														&l_desc,
														D3D12_RESOURCE_STATE_GENERIC_READ,
														nullptr,
														IID_PPV_ARGS(a_outUpload.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "アップロードバッファの作成に失敗しました。");
		return false;
	}

	return true;
}