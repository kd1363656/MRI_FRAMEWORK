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

	if (!UploadTextureSubresources(l_textureResource,
								   a_scratchImage,
								   a_device,
								   a_uploadSystem))
	{
		assert(false && "テクスチャアップロード処理に失敗したため、テクスチャアップロード処理に失敗しました。。");
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
		assert(false && "CPUOnnlySRVからShaderVisibleSRVへのコピーに失敗したため、テクスチャアップロード処理に失敗しました。");
		return false;
	}

	a_textureRecord.m_textureResource = l_textureResource;

	return true;
}

bool FWK::Graphics::TextureUploader::CreateTextureResource(const DirectX::TexMetadata& a_texMetadata, const Device& a_device, TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource)
{
	a_textureResource.Reset();

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
															  static_cast<UINT>(a_texMetadata.height),	
															  static_cast<UINT16>(a_texMetadata.arraySize),
															  static_cast<UINT16>(a_texMetadata.mipLevels));

	return true;
}

bool FWK::Graphics::TextureUploader::UploadTextureSubresources(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
															   const DirectX::ScratchImage&				 a_scratchImage,
															   const Device								 a_device,
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
	return true;
}