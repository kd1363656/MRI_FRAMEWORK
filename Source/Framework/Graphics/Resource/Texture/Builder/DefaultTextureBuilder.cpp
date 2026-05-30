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


	FWK_ASSERT_RETURN_VALUE_IF(!CreateDefaultTextureScratchImage(l_defaultTextureCreateDesc, l_scratchImage), "デフォルトテクスチャ用ScratchImageの作成に失敗しました。", false)

	const auto& l_texMetadata = l_scratchImage.GetMetadata();

	FWK_ASSERT_RETURN_VALUE_IF(!a_textureBatchUploadRecordBuilder.CreateTextureBatchUploadRecord(l_scratchImage,
																								 l_texMetadata,
																								 a_device,
																								 a_gpuMemoryAllocator,
																								 std::wstring(l_defaultTextureCreateDesc.m_name),
																								 Constant::k_invalidStorageID,
																								 a_srvDescriptorPool,
																								 a_textureBatchUploadRecord),
																								 "デフォルトテクスチャ用TextureBatchUploadRecordの作成に失敗しました。",
																								 false)

	return true;
}

bool FWK::Graphics::DefaultTextureBuilder::CreateDefaultTextureScratchImage(const DefaultTextureCreateDesc& a_defaultTextureCreateDesc, DirectX::ScratchImage& a_scratchImage) const
{
	const auto l_result = a_scratchImage.Initialize2D(a_defaultTextureCreateDesc.m_format,
													  k_defaultTextureWidth,
													  k_defaultTextureHeight,
													  k_defaultTextureArraySize,
													  k_defaultTextureMIPLevels);

	FWK_ASSERT_RETURN_VALUE_IF(FAILED(l_result), "DirectXTex::ScratchImage::Initialize2Dに失敗しました。", false)

	// GetImage(MIPレベル番号、
	//			Texture配列番号、
	//			Texture用のスライス番号);
	const auto* l_image = a_scratchImage.GetImage(k_defaultTextureMIPIndex, k_defaultTextureItemIndex, k_defaultTextureSliceIndex);

	FWK_ASSERT_RETURN_VALUE_IF(!l_image, "デフォルトテクスチャ用Imageの取得に失敗しました。", false)

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
				{
					k_defaultTextureWhiteChannel,
					k_defaultTextureWhiteChannel,
					k_defaultTextureWhiteChannel,
					k_defaultTextureAlphaChannel,
				},
				DXGI_FORMAT_R8G8B8A8_UNORM,
				k_defaultBaseColorTextureFilePath,
			};
			
		break;

		case Enum::DefaultTextureType::Normal:
		{
			return
			{
				{
					k_defaultTextureNormalXChannel,
					k_defaultTextureNormalYChannel,
					k_defaultTextureNormalZChannel,
					k_defaultTextureAlphaChannel,
				},
				DXGI_FORMAT_R8G8B8A8_UNORM,
				k_defaultNormalTextureFilePath,
			};
		}
		break;

		default:
			FWK_ASSERT_RETURN_VALUE("未対応のDefaultTextureTypeです。", {})
	}
}