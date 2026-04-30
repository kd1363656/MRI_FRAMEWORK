#include "TextureUploader.h"

bool FWK::Graphics::TextureUploader::UploadTexture(const DirectX::ScratchImage&             a_scratchImage, 
												   const DirectX::TexMetadata&              a_texMetadata,
												   const Device&				            a_device,
												   const GPUMemoryAllocator&				a_gpuMemoryAllocator,
														 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap, 
														 UploadSystem&						a_uploadSystem,
														 Struct::TextureRecord&				a_textureRecord)
{
	if (a_textureRecord.m_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDが無効のため、テクスチャアップロード処理に失敗しました。");
		return false;
	}

	if (a_textureRecord.m_srvIndex == Constant::k_invalidDescriptorHeapIndex)
	{
		assert(false && "SRVインデックスが無効のため、テクスチャアップロード処理に失敗しました。");
		return false;
	}

	TypeAlias::ComPtr<ID3D12Resource2>     l_textureResource = nullptr;
	TypeAlias::ComPtr<D3D12MA::Allocation> l_allocation      = nullptr;
	
	if (!CreateTextureResource(a_texMetadata,
							   a_device,
							   a_gpuMemoryAllocator,
							   l_textureResource,
							   l_allocation))
	{
		assert(false && "TextureResource作成に失敗したため、テクスチャアップロード処理に失敗しました。");
		return false;
	}
	
	if (!UploadTextureSubresources(l_textureResource,
								   a_scratchImage,
								   a_device,
								   a_uploadSystem))
	{
		assert(false && "テクスチャサブリソースアップロード処理に失敗したため、テクスチャアップロード処理に失敗しました。");
		return false;
	}

	if (!CreateTextureSRV(l_textureResource,
						  a_texMetadata,
						  a_textureRecord.m_srvIndex,
						  a_device,
						  a_srvDescriptorHeap))
	{
		assert(false && "テクスチャSRV作成に失敗したため、テクスチャアップロード処理に失敗しました。");
		return false;
	}

	if (!a_srvDescriptorHeap.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(a_textureRecord.m_srvIndex, a_device))
	{
		assert(false && "CPUOnlySRVからShaderVisibleSRVへのコピーに失敗したため、テクスチャアップロード処理に失敗しました。");
		return false;
	}

	a_textureRecord.m_textureResource = l_textureResource;
	a_textureRecord.m_allocation	  = l_allocation;

	return true;
}

bool FWK::Graphics::TextureUploader::CreateTextureResource(const DirectX::TexMetadata&                   a_texMetadata,
														   const Device&			                     a_device,
														   const GPUMemoryAllocator&					 a_gpuMemoryAllocator,
																 TypeAlias::ComPtr<ID3D12Resource2>&     a_textureResource, 
																 TypeAlias::ComPtr<D3D12MA::Allocation>& a_allocation)
{
	a_textureResource.Reset();
	a_allocation.Reset	   ();

	// フォーマットが無効の場合return;
	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "テクスチャフォーマットが無効のため、テクスチャリソース作成に失敗しました。");
		return false;
	}

	// テクスチャの種類がTexture2D以外の場合return;
	if (a_texMetadata.dimension != DirectX::TEX_DIMENSION_TEXTURE2D)
	{
		assert(false && "現在のテクスチャリソース作成はTexture2Dのみ対応しています。");
		return false;
	}

	// DXGI_SAMPLE_DESCについての説明
	// Count   : MSAAのサンプル数
	// Quality : MSAAの品質レベル

	// D3D12_RESOURCE_DESCについての説明
	// Dimension        : リソースの種類
	// Alignment        : リソース配置時のアライメント
	// Width            : リソースの幅
	// Height           : リソースの高さ
	// DepthOrArraySize : Texture3Dでは奥行き、Texture1D / Texture2Dでは配列要素数
	// MipLevels        : mipmapの段数
	// Format           : リソースの画素フォーマット
	// SampleDesc		: MSAA用のパラメータ
	// Layout           : メモリ上の配置方法
	// Flags            : リソースの追加用途や制限

	// Texture2D用のD3D12_RESOURCE_DESCを作成する関数
	// CD3DX12_RESOURCE_DESC::Tex2D(テクスチャの画素フォーマット、
	//								テクスチャの横幅、
	//							    テクスチャの縦幅、
	//							    配列テクスチャの要素数、
	//							    mipmapの段数);

	//// 通常TextureはRenderTarget / DepthStencilではないためClearValueは使用しない
	//// TextureResourceはUploadBufferからCopyTextureRegionで画像データをコピーされるため、
	//// 作成直後のResourceStateはCOPY_DESTにしておく
	if (const auto& l_textureResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(a_texMetadata.format,
															             a_texMetadata.width,
															             static_cast<UINT>  (a_texMetadata.height),
															             static_cast<UINT16>(a_texMetadata.arraySize),
															             static_cast<UINT16>(a_texMetadata.mipLevels));
		!a_gpuMemoryAllocator.CreateTextureResource(l_textureResourceDesc,
													nullptr,
													k_initialTextureResourceState,
													a_textureResource,
													a_allocation))
	{
		assert(false && "D3D12MAによるTextureResource作成処理に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::TextureUploader::UploadTextureSubresources(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
															   const DirectX::ScratchImage&				 a_scratchImage,
															   const Device&							 a_device,
																	 UploadSystem&						 a_uploadSystem) const
{
	if (!a_textureResource)
	{
		assert(false && "TextureResourceが無効のため、テクスチャサブリソースアップロード処理に失敗しました。");
		return false;
	}

	if (a_scratchImage.GetImageCount() == 0ULL)
	{
		assert(false && "ScratchImageの画像数が0のため、テクスチャサブリソースアップロード処理に失敗しました。");
		return false;
	}

	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、テクスチャサブリソースアップロード処理に失敗しました。");
		return false;
	}

	const auto& l_textureResourceDesc = a_textureResource->GetDesc();

	const auto l_subresourceCount = static_cast<UINT>(a_scratchImage.GetImageCount());

	// リストをサブリソース数分確保する
	auto l_layoutList         = std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>(l_subresourceCount);
	auto l_rowCountList       = std::vector<UINT>							   (l_subresourceCount);
	auto l_rowSizeInBytesList = std::vector<UINT64>							   (l_subresourceCount);

	UINT64 l_requiredUploadBufferSize = 0ULL;

	// GetCopyableFootprints(コピー先TextureResourceの設定、	
	// 　　　　　　　　　　　計算を開始するサブリソース番号、
	//						 計算するサブリソース数、
	//	                     UploadBuffer内の開始オフセット、
	//						 サブリソースごとのUploadBuffer配置情報の受取先、
	//					     サブリソースごとの行数の受取先、
	//                       サブリソースごとの1行当たりの受取先、
	//					     必要なUploadBufferサイズの受取先);
	l_device->GetCopyableFootprints(&l_textureResourceDesc,
									Constant::k_firstSubresourceIndex,
									l_subresourceCount,
									0ULL,
								    l_layoutList.data(),
									l_rowCountList.data(),
									l_rowSizeInBytesList.data(),
									&l_requiredUploadBufferSize);

	UploadBuffer l_uploadBuffer = {};

	if (!l_uploadBuffer.Create(l_requiredUploadBufferSize, a_device))
	{
		assert(false && "テクスチャ用UploadBufferの作成に失敗したため、テクスチャサブリソースアップロード処理に失敗しました。");
		return false;
	}

	auto* l_mappedData = l_uploadBuffer.Map();

	if (!l_mappedData)
	{
		assert(false && "UploadBufferのMapに失敗したため、テクスチャサブリソースアップロード処理に失敗しました。");
		return false;
	}

	const auto* l_imageList = a_scratchImage.GetImages();

	if (!l_imageList)
	{
		assert(false && "ScratchImageの画像データ取得に失敗したため、テクスチャサブリソースアップロード処理に失敗しました。");
		
		// アップロードバッファのマッピングを解除
		l_uploadBuffer.UnMap();
		return false;
	}

	for (UINT l_subresourceIndex = 0U; l_subresourceIndex < l_subresourceCount; ++l_subresourceIndex)
	{
		const auto& l_image  = l_imageList [l_subresourceIndex];
		const auto& l_layout = l_layoutList[l_subresourceIndex];

		auto* l_destinationSubresource = l_mappedData + l_layout.Offset;

		const auto  l_destinationRotPitch   = l_layout.Footprint.RowPitch;
		const auto& l_destinationSlicePitch = l_destinationRotPitch * static_cast<std::size_t>(l_rowCountList[l_subresourceIndex]);

		const auto& l_sourceRotPitch   = l_image.rowPitch;
		const auto& l_sourceSlicePitch = l_image.slicePitch;

		const auto& l_copyRowSize = l_rowSizeInBytesList[l_subresourceIndex];

		for (UINT l_depthIndex = 0U; l_depthIndex < l_layout.Footprint.Depth; ++l_depthIndex)
		{
			for (UINT l_rowIndex = 0U; l_rowIndex < l_rowCountList[l_subresourceIndex]; ++l_rowIndex)
			{
				auto* l_destination = l_destinationSubresource + l_depthIndex * l_destinationSlicePitch + l_rowIndex * l_destinationRotPitch;
			
				const auto* l_source = l_image.pixels + l_depthIndex * l_sourceSlicePitch + l_rowIndex * l_sourceRotPitch;

				std::memcpy(l_destination, l_source, l_copyRowSize);
			}
		}
	}

	l_uploadBuffer.UnMap();

	Struct::TextureUploadRecord l_textureUploadRecord = {};

	l_textureUploadRecord.m_textureResource = a_textureResource;
	l_textureUploadRecord.m_uploadBuffer    = l_uploadBuffer;
	l_textureUploadRecord.m_layoutList      = l_layoutList;

	std::vector<Struct::TextureUploadRecord> l_textureUploadRecordList = {};

	l_textureUploadRecordList.emplace_back(l_textureUploadRecord);

	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(l_textureUploadRecordList))
	{
		assert(false && "UploadSystemへのテクスチャコピー送信に失敗したため、テクスチャサブリソースアップロード処理に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::TextureUploader::CreateTextureSRV(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
													  const DirectX::TexMetadata&				a_texMetadata,
													  const UINT								a_srvIndex,
													  const Device&							    a_device,
															DescriptorPool<SRVDescriptorHeap>&  a_srvDescriptorHeap)
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

	if (a_texMetadata.dimension != DirectX::TEX_DIMENSION_TEXTURE2D)
	{
		assert(false && "現在のTextureSRV作成処理はTexture2Dのみ対応しています。");
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

	// D3D12_SHADER_RESOURCE_VIEW_DESCについて
	// Shader4ComponentMapping : シェーダーでTextureを読むときのRGBA成分の対応指定
	// Format                  : SRVとして参照するときのDXGI_FORMAT
	// ViewDimension		   : シェーダーからTexture2DArrayとして参照する指定
	// MostDetailedMIP         : 参照を開始するMIPLevel
	// MIPLevels	           : 参照可能なMIPLevel数
	// FirstArraySlice         : 参照を開始する配列番号
	// ArraySize	           : 参照可能な配列数
	// PlaneSlice	           : 通常Textureでは0
	// ResourceMINLODClamp     : 参照する最小LODの制限
	l_srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	l_srvDesc.Format				  = a_texMetadata.format;

	// 1より大きければ配列テクスチャ
	if (a_texMetadata.arraySize >= k_texture2DArrayMINArraySize)
	{
		// Texture2DArrayとしてSRVを作成する
		l_srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

		l_srvDesc.Texture2DArray.MostDetailedMip     = k_mostDetailedMIP;
		l_srvDesc.Texture2DArray.MipLevels		     = static_cast<UINT>(a_texMetadata.mipLevels);
		l_srvDesc.Texture2DArray.FirstArraySlice     = k_firstArraySlice;
		l_srvDesc.Texture2DArray.ArraySize           = static_cast<UINT>(a_texMetadata.arraySize);
		l_srvDesc.Texture2DArray.PlaneSlice          = k_planeSlice;
		l_srvDesc.Texture2DArray.ResourceMinLODClamp = k_resourceMINLODClamp;
	}
	else
	{
		// 通常Texture2DとしてSRVを作成する
		l_srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

		l_srvDesc.Texture2D.MostDetailedMip     = k_mostDetailedMIP;
		l_srvDesc.Texture2D.MipLevels		    = static_cast<UINT>(a_texMetadata.mipLevels);
		l_srvDesc.Texture2D.PlaneSlice		    = k_planeSlice;
		l_srvDesc.Texture2D.ResourceMinLODClamp = k_resourceMINLODClamp;
	}

	const auto& l_cpuOnlyCPUHandle = a_srvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_srvIndex);

	// CreateShaderResourceView(対象Resource,
	//							SRVとしての見え方設定、
	//							SRVを書き込むCPUDescriptorHandle);
	// CPUOnly側のDescriptorHeapにSRVを作成しておき、
	// 後でCopyCPUOnlyDescriptorToShaderVisibleDescriptorでShaderVisible側へコピーする
	l_device->CreateShaderResourceView(a_textureResource.Get(), &l_srvDesc, l_cpuOnlyCPUHandle);

	return true;
}