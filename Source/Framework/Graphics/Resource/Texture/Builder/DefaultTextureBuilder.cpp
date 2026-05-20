#include "DefaultTextureBuilder.h"

bool FWK::Graphics::DefaultTextureBuilder::CreateDefaultTextureBatchUploadRecord(const Device&			                  a_device, 
																				 const GPUMemoryAllocator&                a_gpuMemoryAllocator, 
																				 const TextureBatchUploadRecordBuilder&   a_textureBatchUploadRecordBuilder,
																				 const Enum::DefaultTextureType			  a_defaultTextureType,
																					   DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
																					   Struct::TextureBatchUploadRecord&  a_textureBatchUploadRecord) const
{
	const auto l_defaultTextureCreateDesc = GetVALDefaultTextureCreateDesc(a_defaultTextureType);

	DirectX::ScratchImage l_scratchImage = {};

	if (!CreateDefaultTextureScratchImage(l_defaultTextureCreateDesc, l_scratchImage))
	{
		assert(false && "デフォルトテクスチャ用ScratchImageの作成に失敗しました。");
		return false;
	}

	if (const auto& l_texMetadata = l_scratchImage.GetMetadata();
		!a_textureBatchUploadRecordBuilder.CreateTextureBatchUploadRecord(l_scratchImage,
																	      l_texMetadata,
																	      a_device,
																	      a_gpuMemoryAllocator,
																	      std::wstring(l_defaultTextureCreateDesc.m_name),
																	      Constant::k_invalidStorageID,
																	      a_srvDescriptorPool,
																	      a_textureBatchUploadRecord))
	{
		assert(false && "デフォルトテクスチャ用TextureBatchUploadRecordの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::DefaultTextureBuilder::CreateDefaultTextureScratchImage(const DefaultTextureCreateDesc& a_defaultTextureCreateDesc, DirectX::ScratchImage& a_scratchImage) const
{
	const auto l_result = a_scratchImage.Initialize2D(a_defaultTextureCreateDesc.m_format,
													  k_defaultTextureWidth,
													  k_defaultTextureHeight,
													  k_defaultTextureArraySize,
													  k_defaultTextureMIPLevels);

	if (FAILED(l_result))
	{
		assert(false && "DirectXTex::ScratchImage::Initialize2Dに失敗しました。");
		return false;
	}

	// GetImage(MIPレベル番号、
	//			Texture配列番号、
	//			Texture用のスライス番号);
	const auto* l_image = a_scratchImage.GetImage(k_defaultTextureMIPIndex, k_defaultTextureItemIndex, k_defaultTextureSliceIndex);

	if (!l_image)
	{
		assert(false && "デフォルトテクスチャ用Imageの取得に失敗しました。");
		return false;
	}

	std::memcpy(l_image->pixels, a_defaultTextureCreateDesc.m_pixel.data(), a_defaultTextureCreateDesc.m_pixel.size());

	return true;
}

FWK::Graphics::DefaultTextureBuilder::DefaultTextureCreateDesc FWK::Graphics::DefaultTextureBuilder::GetVALDefaultTextureCreateDesc(const Enum::DefaultTextureType a_defaultTextureType) const
{
	switch (a_defaultTextureType)
	{
		case Enum::DefaultTextureType::BaseColor:

			return
			{
				.m_pixel =
				{
					Constant::k_defaultTextureWhiteChannel,
					Constant::k_defaultTextureWhiteChannel,
					Constant::k_defaultTextureWhiteChannel,
					Constant::k_defaultTextureAlphaChannel,
				},
				.m_format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.m_name   = Constant::k_defaultBaseColorTextureFilePath,
			};
			
		break;

		case Enum::DefaultTextureType::Normal:
		{
			return
			{
				.m_pixel =
				{
					Constant::k_defaultTextureNormalXChannel,
					Constant::k_defaultTextureNormalYChannel,
					Constant::k_defaultTextureNormalZChannel,
					Constant::k_defaultTextureAlphaChannel,
				},
				.m_format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.m_name   = Constant::k_defaultNormalTextureFilePath,
			};
		}
		break;

		default:
			assert(false && "未対応のDefaultTextureTypeです。");
			return {};
	}
}