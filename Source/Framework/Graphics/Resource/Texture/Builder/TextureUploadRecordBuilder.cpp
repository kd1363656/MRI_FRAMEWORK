#include "TextureUploadRecordBuilder.h"

bool FWK::Graphics::TextureUploadRecordBuilder::CreateTextureUploadRecord(const DirectX::ScratchImage&             a_scratchImage, 
																		  const DirectX::TexMetadata&              a_texMetadata,
																		  const Device&			                   a_device,
																		  const GPUMemoryAllocator&	               a_gpuMemoryAllocator,
																		  const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
																				Struct::TextureRecord&			   a_textureRecord,
																				Struct::TextureUploadRecord&	   a_textureUploadRecord) const
{
	if (a_textureRecord.m_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDが無効のため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	if (a_textureRecord.m_srvIndex == Constant::k_invalidDescriptorHeapIndex)
	{
		assert(false && "SRVインデックスが無効のため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	// DEFAULTヒープ上で管理するテクスチャ本体とテクスチャのヒープ領域を管理するアロケータを
	// 格納するための変数を定義
	TypeAlias::ComPtr<ID3D12Resource2>     l_textureResource = nullptr;
	TypeAlias::ComPtr<D3D12MA::Allocation> l_allocation		 = nullptr;

	// TexMetadataの情報をもとに、DEFAULTヒープ上へ配置するTextureResourceを作成する
	if (!CreateTextureResource(a_texMetadata,
							   a_gpuMemoryAllocator,
							   l_textureResource,
							   l_allocation))
	{
		assert(false && "TextureResource作成に失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	// ScratchImageの画像データをUploadBufferへ書き込み、UploadSystemへ渡すコピー情報を作成する
	// ここではCopyCommandQueueへ送信しない
	// TextureSystem側で複数TextureUploadRecordをためて、最後にまとめてSubmitする
	if (!CreateTextureSubresourcesUploadRecord(l_textureResource,
											   a_scratchImage,
											   a_device,
											   a_textureUploadRecord))
	{
		assert(false && "テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	// 作成したTextureResourceをシェーダーから参照できるように、CPUOnly側のDescriptorHeapへSRVを作成する
	if (!CreateTextureSRV(l_textureResource,
						  a_texMetadata,
						  a_textureRecord.m_srvIndex,
						  a_device,
						  a_srvDescriptorPool))
	{
		assert(false && "TextureSRV作成に失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	// CPUOnly側に作成したSRVをShaderVisible側へコピーする
	if (!a_srvDescriptorPool.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(a_textureRecord.m_srvIndex, a_device))
	{
		assert(false && "CPUOnlyからShaderVisibleSRVへのコピーに失敗したため、テクスチャアップロード情報作成処理に失敗しました。");
		return false;
	}

	a_textureRecord.m_textureResource = l_textureResource;
	a_textureRecord.m_allocation      = l_allocation;

	return true;
}

bool FWK::Graphics::TextureUploadRecordBuilder::CreateTextureResource(const DirectX::TexMetadata&                   a_texMetadata,
																	  const GPUMemoryAllocator&                     a_gpuMemoryAllocator,
																			TypeAlias::ComPtr<ID3D12Resource2>&     a_textureResource,
																			TypeAlias::ComPtr<D3D12MA::Allocation>& a_allocation) const
{
	a_textureResource.Reset();
	a_allocation.Reset     ();

	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "テクスチャフォーマットが無効のため、TextureResource作成処理に失敗しました。");
		return false;
	}

	if (a_texMetadata.dimension != DirectX::TEX_DIMENSION_TEXTURE2D)
	{
		assert(false && "現在のTextureResource作成処理はTexture2Dのみ対応しています。");
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
																	    static_cast<UINT>  (a_texMetadata.height),
																	    static_cast<UINT16>(a_texMetadata.arraySize),
																	    static_cast<UINT16>(a_texMetadata.mipLevels));

		// CopyTextureRegionのコピー先として使うため、初期状態はCOPY_DESTにする
		!a_gpuMemoryAllocator.CreateTextureResource(l_textureResourceDesc,
													nullptr,
													D3D12_RESOURCE_STATE_COPY_DEST,
													a_textureResource,
													a_allocation))
	{
		assert(false && "D3D12MAによるTextureResource作成処理に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::TextureUploadRecordBuilder::CreateTextureSubresourcesUploadRecord(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, 
																					  const DirectX::ScratchImage&			    a_scratchImage,
																					  const Device&							    a_device,
																						    Struct::TextureUploadRecord&	    a_textureUploadRecord) const
{
	if (!a_textureResource)
	{
		assert(false && "TextureResourceが無効のため、テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	if (a_scratchImage.GetImageCount() == k_emptyScratchImageCount)
	{
		assert(false && "ScratchImageの画像数が0のため、テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、テクスチャサブリソースアップロード情報作成処理に失敗しました。");
		return false;
	}

	const auto& l_textureResourceDesc = a_textureResource->GetDesc();
	const auto  l_subresourceCount    = static_cast<UINT>         (a_scratchImage.GetImageCount());

	auto l_layoutList         = std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>(l_subresourceCount);
	auto l_rowCountList       = std::vector<UINT>							   (l_subresourceCount);
	auto l_rowSizeInBytesList = std::vector<UINT64>							   (l_subresourceCount);

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

	UploadBuffer l_uploadBuffer = {};

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

	for (UINT l_subresourceIndex = 0U; l_subresourceIndex < l_subresourceCount; ++l_subresourceIndex)
	{
		const auto& l_image  = l_imageList [l_subresourceIndex];
		const auto& l_layout = l_layoutList[l_subresourceIndex];

		// 現在のサブリソースを書き込むUploadBuffer上の先頭アドレス
		auto* l_destinationSubresource = l_mappedData + l_layout.Offset;

		// UploadBuffer側のRowPitchにD3D12のアライメント用の余白が含まれる
		const auto  l_destinationRowPitch   = l_layout.Footprint.RowPitch;
		const auto& l_destinationSlicePitch = l_destinationRowPitch * static_cast<std::size_t>(l_rowCountList[l_subresourceIndex]);

		const auto& l_sourceRowPitch   = l_image.rowPitch;
		const auto& l_sourceSlicePitch = l_image.slicePitch;

		// 実際にコピーする1行当たりの有効データサイズ
		// UploadBuffer側のRowPitch全体ではなく、有効な画像データ部分だけコピーする
		const auto l_copyRowSize = l_rowSizeInBytesList[l_subresourceIndex];

		// 一行ずつ画像をコピーする、
		// コピー元とコピー先で1行当たりの間隔、つまりRowPitchが違う可能性があるから
		for (UINT l_depthIndex = 0U; l_depthIndex < l_layout.Footprint.Depth; ++l_depthIndex)
		{
			for (UINT l_rowIndex = 0U; l_rowIndex < l_rowCountList[l_subresourceIndex]; ++l_rowIndex)
			{
					  auto* l_destination = l_destinationSubresource + l_depthIndex * l_destinationSlicePitch + l_rowIndex * l_destinationRowPitch;
				const auto* l_source      = l_image.pixels			 + l_depthIndex * l_sourceSlicePitch      + l_rowIndex * l_sourceRowPitch;

				std::memcpy(l_destination, l_source, l_copyRowSize);
			}
		}
	}

	l_uploadBuffer.UnMap();

	// UploadSystemに渡すテクスチャコピー情報を生成する
	// UploadBufferはGPUコピーが完了するまで生存している必要がある
	// SubmitTextureCopyBatchAndWait()でコピー完了まで待つため、このRecord内で保持する
	a_textureUploadRecord.m_textureResource = a_textureResource;
	a_textureUploadRecord.m_uploadBuffer	= std::move(l_uploadBuffer);
	a_textureUploadRecord.m_layoutList      = std::move(l_layoutList);

	return true;
}

bool FWK::Graphics::TextureUploadRecordBuilder::CreateTextureSRV(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, 
																 const DirectX::TexMetadata&			   a_texMetadata,
																 const UINT								   a_srvIndex, 
																 const Device&							   a_device,
																 const DescriptorPool<SRVDescriptorHeap>&  a_srvDescriptorPool) const
{
	if (!a_textureResource)
	{
		assert(false && "TextureResourceが無効のため、TextureSRV作成処理に失敗しました。");
		return false;
	}

	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "テクスチャフォーマットが無効のため、TextureSRV作成処理に失敗しました。");
		return false;
	}

	if (a_srvIndex == Constant::k_invalidDescriptorHeapIndex)
	{
		assert(false && "SRVインデックスが無効のため、TextureSRV作成処理に失敗しました。");
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

	const auto& l_cpuOnlyCPUHandle = a_srvDescriptorPool.FetchVALCPUOnlyCPUHandle(a_srvIndex);

	// 作成したビューを用いてTextureResourceとSRVを結び付ける
	l_device->CreateShaderResourceView(a_textureResource.Get(), &l_srvDesc, l_cpuOnlyCPUHandle);

	return true;
}