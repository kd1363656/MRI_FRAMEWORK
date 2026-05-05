#include "TextureBatchUploadRecordBuilder.h"

bool FWK::Graphics::TextureBatchUploadRecordBuilder::CreateTextureBatchUploadRecord(const DirectX::ScratchImage&             a_scratchImage,
																					const DirectX::TexMetadata&              a_texMetadata, 
																					const Device&			                 a_device,
																					const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																					const std::wstring&						 a_filePath,
																						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
																						  TextureIDAllocator&				 a_textureIDAllocator,
																						  Struct::TextureBatchUploadRecord&  a_textureBatchUploadRecord)
{
	// まずはGPU側用のテクスチャリソースのヒープ領域を確保
	if (!CreateTextureResource(a_texMetadata, a_gpuMemoryAllocator, a_textureBatchUploadRecord.m_textureRecord))
	{
		assert(false && "TextureResource作成処理に失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	// ScratchImageの画像データをUploadBufferへ書き込み、UploadSystemへ渡すコピー情報を作成する
	// ここではCopyCommandQueueへ送信しない(バッチ処理を行うため)
	if (!CreateTextureUploadRecord(a_scratchImage, a_device, a_textureBatchUploadRecord))
	{
		assert(false && "テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	// 作成したTextureResourceをシェーダーから参照できるように、CPUOnly側のDescriptorHeapへSRVを作成する
	if (!CreateTextureSRV(a_texMetadata,
						  a_device,
						  a_srvDescriptorPool,
						  a_textureBatchUploadRecord.m_textureRecord))
	{
		assert(false && "TextureSRV作成に失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	auto& l_textureRecord = a_textureBatchUploadRecord.m_textureRecord;

	// CPUOnlyに作成したSRVをShaderVisible側へコピーする
	if (!a_srvDescriptorPool.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(l_textureRecord.m_srvIndex, a_device))
	{
		assert(false && "CPUOnlyからshaderVisibleSRVへのコピーに失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	const auto l_textureID = a_textureIDAllocator.Allocate();

	if (l_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDの割り当てに失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	// 最後にD3D12_RESOURCE_STATESと参照カウント、ファイルパスを格納する
	l_textureRecord.m_currentState      = D3D12_RESOURCE_STATE_COMMON;
	l_textureRecord.m_retiredFenceValue = Constant::k_unusedFenceValue;
	l_textureRecord.m_referenceCount    = k_initialTextureReferenceCount;
	l_textureRecord.m_textureID         = l_textureID;
	l_textureRecord.m_filePath          = a_filePath;

	return true;
}

bool FWK::Graphics::TextureBatchUploadRecordBuilder::CreateTextureResource(const DirectX::TexMetadata& a_texMetadata, const GPUMemoryAllocator& a_gpuMemoryAllocator, Struct::TextureRecord& a_textureRecord) const
{
	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "テクスチャフォーマットが無効のため、TextureResource作成処理に失敗しました。");
		return false;
	}

	if (a_texMetadata.dimension != DirectX::TEX_DIMENSION_TEXTURE2D)
	{
		assert(false && "TextureResource作成処理はTexture2Dのみ対応しています。");
		return false;
	}

	// Texture2D用のD3D12_RESOURCE_DESCを作成する
	// Tex2D(フォーマット、
	//		 横幅、
	//		 縦幅、
	//		 配列数、
	//		 MIP数);

	if (const auto l_textureResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(a_texMetadata.format,
																	    a_texMetadata.width,
																	    static_cast<UINT>(a_texMetadata.height),
																	    static_cast<UINT16>(a_texMetadata.arraySize),
																	    static_cast<UINT16>(a_texMetadata.mipLevels));

		// CopyCommandQueueでCopyTextureRegionをするために、初期状態はCOMMONにする、
		// COMMON状態のTextureResourceは、CopyCommandQueue上のCopyTextureRegionで
		// コピー先として使用されると暗黙的にコピー先として扱われる
		!a_gpuMemoryAllocator.CreateTextureResource(l_textureResourceDesc,
													nullptr,
													D3D12_RESOURCE_STATE_COMMON,
													a_textureRecord.m_textureResource,
													a_textureRecord.m_allocation))
	{
		assert(false && "D3D12MAによるTextureResource作成処理に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::TextureBatchUploadRecordBuilder::CreateTextureUploadRecord(const DirectX::ScratchImage& a_scratchImage, const Device& a_device, Struct::TextureBatchUploadRecord& a_textureBatchUploadRecord) const
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	const auto& l_textureRecord = a_textureBatchUploadRecord.m_textureRecord.m_textureResource;

	const auto& l_textureResourceDesc = l_textureRecord->GetDesc();
	const auto  l_subresourceCount    = static_cast<UINT>       (a_scratchImage.GetImageCount());

	auto& l_textureUploadRecord = a_textureBatchUploadRecord.m_textureUploadRecord;

	// 各サブリソースをUploadBufferのどこに置くかを表す
	auto& l_layoutList = l_textureUploadRecord.m_layoutList;

	// 念のため要素をclear()してからサブリソース分の容量を確保
	l_layoutList.clear ();
	l_layoutList.resize(static_cast<size_t>(l_subresourceCount));

	// 各サブリソースの行数
	auto l_rowCountList = std::vector<UINT>(l_subresourceCount);

	// 1行当たりの実データサイズ
	auto l_rowSizeInBytesList = std::vector<UINT64>(l_subresourceCount);

	UINT64 l_requiredUploadBufferSize = k_initialRequiredUploadBufferSize;

	// TextureResourceへコピーするために必要なUploadBuffer上の配置情報を計算する
	// GetCopyableFootprints(コピー先TextureResourceの設定、
	//						 計算を開始するサブリソース番号、
	//						 UploadBuffer内の配置情報計算開始オフセット、
	//						 サブリソースごとのUploadBuffer配置情報の受取先、
	//						 サブリソースごとの行数の受取先、
	//						 サブリソースごとの1行当たりの有効データサイズの受取先
	//						 必要なUploadBufferサイズの受取先);

	l_device->GetCopyableFootprints(&l_textureResourceDesc,
									Constant::k_firstSubresourceIndex,
									l_subresourceCount,
									k_uploadBufferBeginOffset,
									l_layoutList.data(),
									l_rowCountList.data(),
									l_rowSizeInBytesList.data(),
									&l_requiredUploadBufferSize);

	auto& l_uploadBuffer        = l_textureUploadRecord.m_uploadBuffer;

	// DEFAULTヒープ上にあるTextureResourceは直接CPUから書き込むことはできないため
	// CPU書き込み可能なUploadBufferを作成する
	if (!l_uploadBuffer.Create(a_device, l_requiredUploadBufferSize))
	{
		assert(false && "テクスチャ用UploadBufferの作成処理に失敗したため、テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	auto* l_mappedData = l_uploadBuffer.Map();

	if (!l_mappedData)
	{
		assert(false && "UploadBufferのMapに失敗したため、テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	const auto* l_imageList = a_scratchImage.GetImages();

	if (!l_imageList)
	{
		assert(false && "ScratchImageの画像データ取得に失敗したため、テクスチャサブリソースアップロード情報作成処理に失敗しました。");

		// Mapを解除してからreturn
		l_uploadBuffer.UnMap();
		return false;
	}

	// DirectXTexで読み込んだ画像データをUploadBufferへコピーする
	// 各サブリソースひとつずつに対して実行
	for (UINT l_subresourceIndex = 0U; l_subresourceIndex < l_subresourceCount; ++l_subresourceIndex)
	{
		// 現在処理するサブリソースの元画像データと、UploadBuffer上の配置情報を取得する
		const auto& l_image  = l_imageList [l_subresourceIndex];
		const auto& l_layout = l_layoutList[l_subresourceIndex];

		// 現在のサブリソースを書き込むUploadBuffer上の先頭アドレスを取得する
		// l_layout.OffsetはGetCopyableFootprints()が計算したサブリソースごとの開始位置
		auto* l_destinationSubresource = l_mappedData + l_layout.Offset;

		// UploadBuffer側で1行進むためのバイト数を取得する
		// RowPitchにはD3D12のコピー条件に合わせてアライメントされた値になる
		const auto l_destinationRowPitch = l_layout.Footprint.RowPitch;

		// UploadBuffer側でDepth方向に1枚進むためのバイト数を計算する
		// Texture2DではDepthは基本1だが、計算式としては1枚分 = RowPitch * 行数
		const auto& l_destinationSlicePitch = l_destinationRowPitch * static_cast<std::size_t>(l_rowCountList[l_subresourceIndex]);

		// ScratchImage側で1行進むためのバイト数を取得する
		const auto& l_sourceRowPitch = l_image.rowPitch;

		// ScratchImage側でDepth方向に1枚進むためのバイト数を取得する
		const auto& l_sourceSlicePitch = l_image.slicePitch;

		// 実際にコピーする1行当たりの有効データサイズ
		// UploadBuffer側のRowPitch全体ではなく、有効な画像データ部分だけコピーする
		// (RowPitchでアライメントするための余白を含んでいないということ)
		const auto l_copyRowSize = l_rowSizeInBytesList[l_subresourceIndex];

		// Depth方向にコピーする
		for (UINT l_depthIndex = k_initialLayoutDepthIndex; l_depthIndex < l_layout.Footprint.Depth; ++l_depthIndex)
		{
			// RowPitchがコピー元とコピー先で異なる可能性があるため、1行ずつコピーする
			for (UINT l_rowIndex = k_initialRowIndex; l_rowIndex < l_rowCountList[l_subresourceIndex]; ++l_rowIndex)
			{
				// UploadBuffer側の現在の行の書き込み先アドレスを計算する
				auto* l_destination = l_destinationSubresource + l_depthIndex * l_destinationSlicePitch + l_rowIndex * l_destinationRowPitch;

				// ScratchImage側の現在の行の読み取り元アドレスを計算する
				const auto* l_source = l_image.pixels + l_depthIndex * l_sourceSlicePitch + l_rowIndex * l_sourceRowPitch;

				// ScratchImage側の1行分の有効データをUploadBuffer側の対応する行へコピーする
				std::memcpy(l_destination, l_source, l_copyRowSize);
			}
		}
	}

	// マップを解除
	l_uploadBuffer.UnMap();

	return true;
}

bool FWK::Graphics::TextureBatchUploadRecordBuilder::CreateTextureSRV(const DirectX::TexMetadata&              a_texMetadata, 
																	  const Device&				               a_device, 
																		    DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
																			Struct::TextureRecord&		       a_textureRecord) const
{
	const auto& l_textureResource = a_textureRecord.m_textureResource;

	if (!l_textureResource)
	{
		assert(false && "TextureResourceが無効のため、TextureSRV作成処理に失敗しました。");
		return false;
	}

	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "テクスチャフォーマットが無効のため、TextureSRV作成処理に失敗しました。");
		return false;
	}

	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、TextureSRV作成処理に失敗しました。");
		return false;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC l_srvDesc = {};

	l_srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	l_srvDesc.Format				  = a_texMetadata.format;

	if (a_texMetadata.arraySize >= k_texture2DArrayMINArraySize)
	{
		l_srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

		l_srvDesc.Texture2DArray.MostDetailedMip     = k_mostDetailedMIP;
		l_srvDesc.Texture2DArray.MipLevels           = static_cast<UINT>(a_texMetadata.mipLevels);
		l_srvDesc.Texture2DArray.FirstArraySlice     = k_firstArraySlice;
		l_srvDesc.Texture2DArray.ArraySize           = static_cast<UINT>(a_texMetadata.arraySize);
		l_srvDesc.Texture2DArray.PlaneSlice			 = k_planeSlice;
		l_srvDesc.Texture2DArray.ResourceMinLODClamp = k_resourceMINLODClamp;
	}
	else
	{
		l_srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

		l_srvDesc.Texture2D.MostDetailedMip     = k_mostDetailedMIP;
		l_srvDesc.Texture2D.MipLevels           = static_cast<UINT>(a_texMetadata.mipLevels);
		l_srvDesc.Texture2D.PlaneSlice		    = k_planeSlice;
		l_srvDesc.Texture2D.ResourceMinLODClamp = k_resourceMINLODClamp;
	}

	// SRVインデックスを格納
	const auto l_srvIndex = a_srvDescriptorPool.Allocate();

	if (l_srvIndex == Constant::k_invalidDescriptorHeapIndex)
	{
		assert(false && "SRV用ディスクリプタインデックスの確保に失敗したため、TextureSRV作成処理に失敗しました。");
		return false;
	}

	a_textureRecord.m_srvIndex = l_srvIndex;

	const auto& l_cpuOnlyCPUHandle = a_srvDescriptorPool.FetchVALCPUOnlyCPUHandle(a_textureRecord.m_srvIndex);

	// 作成したビューを用いてTextureResourceとSRVを結び付ける
	l_device->CreateShaderResourceView(l_textureResource.Get(), &l_srvDesc, l_cpuOnlyCPUHandle);

	return true;
}