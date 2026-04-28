#include "TextureUploader.h"

bool FWK::Graphics::TextureUploader::UploadTexture(const DirectX::ScratchImage&             a_scratchImage, 
												   const DirectX::TexMetadata&              a_texMetadata,
												   const Device&				            a_device,
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

	TypeAlias::ComPtr<ID3D12Resource2> l_textureResource = nullptr;

	Struct::PlacedResourceAllocationRecord l_allocationRecord = {};

	if (!CreateTextureResource(a_texMetadata,
							   a_device,
							   l_textureResource,
							   l_allocationRecord))
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

	a_textureRecord.m_textureResource  = l_textureResource;
	a_textureRecord.m_allocationRecord = l_allocationRecord;

	return true;
}

bool FWK::Graphics::TextureUploader::CreateTextureResource(const DirectX::TexMetadata&                   a_texMetadata,
														   const Device&			                     a_device,
																 TypeAlias::ComPtr<ID3D12Resource2>&     a_textureResource, 
																 Struct::PlacedResourceAllocationRecord& a_allocationRecord)
{
	a_textureResource.Reset();
	a_allocationRecord = {};

	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、テクスチャリソース作成に失敗しました。");
		return false;
	}

	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "テクスチャフォーマットが無効のため、テクスチャリソース作成に失敗しました。");
		return false;
	}

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
	// format    : テクスチャの画素フォーマット
	// width     : テクスチャの横幅
	// height    : テクスチャの縦幅
	// arraySize : 配列テクスチャの要素数
	// mipLevels : mipmapの段数

	auto l_textureResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(a_texMetadata.format,
															  a_texMetadata.width,
															  static_cast<UINT>  (a_texMetadata.height),
															  static_cast<UINT16>(a_texMetadata.arraySize),
															  static_cast<UINT16>(a_texMetadata.mipLevels));

	//// 通常TextureはRenderTarget / DepthStencilではないためClearValueは使用しない
	//// TextureResourceはUploadBufferからCopyTextureRegionで画像データをコピーされるため、
	//// 作成直後のResourceStateはCOPY_DESTにしておく
	//if (!a_texturePlacedResourceAllocator.CreatePlacedResource(l_textureResourceDesc,
	//														   a_device,
	//														   nullptr,
	//														   k_initialTextureResourceState,
	//														   a_textureResource,
	//														   a_allocationRecord))
	//{
	//	assert(false && "PlacedResourceAllocatorによるTextureResource作成に失敗しました。");
	//	return false;
	//}

	return true;
}

bool FWK::Graphics::TextureUploader::UploadTextureSubresources(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
															   const DirectX::ScratchImage&				 a_scratchImage,
															   const Device&							 a_device,
																	 UploadSystem&						 a_uploadSystem) const
{
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