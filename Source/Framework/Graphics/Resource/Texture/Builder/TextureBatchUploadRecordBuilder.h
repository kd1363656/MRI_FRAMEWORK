#pragma once

namespace FWK::Graphics
{
	class TextureSystem;
}

namespace FWK::Graphics
{
	class TextureBatchUploadRecordBuilder final
	{
	public:
		
		 TextureBatchUploadRecordBuilder() = default;
		~TextureBatchUploadRecordBuilder() = default;

		bool CreateTextureBatchUploadRecord(const DirectX::ScratchImage&             a_scratchImage,
											const DirectX::TexMetadata&              a_texMetadata,
											const Device&                            a_device,
											const GPUMemoryAllocator&                a_gpuMemoryAllocator,
											const std::wstring&						 a_filePath,
											const TypeAlias::StorageID				 a_storageID,
												  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
												  Struct::TextureBatchUploadRecord&  a_textureBatchUploadRecord) const;

	private:

		bool CreateTextureResource(const std::weak_ptr<Struct::TextureRecord>& a_textureRecord, const DirectX::TexMetadata& a_texMetadata, const GPUMemoryAllocator& a_gpuMemoryAllocator) const;

		bool CreateTextureUploadRecord(const DirectX::ScratchImage& a_scratchImage, const Device& a_device, Struct::TextureBatchUploadRecord& a_textureBatchUploadRecord) const;

		bool CreateTextureSRV(const std::weak_ptr<Struct::TextureRecord>& a_textureRecord,
							  const DirectX::TexMetadata&                 a_texMetadata,
							  const Device&                               a_device,
									DescriptorPool<SRVDescriptorHeap>&    a_srvDescriptorPool) const;

		static constexpr UINT64 k_uploadBufferBeginOffset         = 0ULL;
		static constexpr UINT64 k_initialRequiredUploadBufferSize = 0ULL;

		static constexpr UINT k_mostDetailedMIP = 0U;
		static constexpr UINT k_planeSlice		= 0U;
		static constexpr UINT k_firstArraySlice = 0U;

		static constexpr FLOAT k_resourceMINLODClamp = 0.0F;

		static constexpr std::size_t k_texture2DArrayMINArraySize = 2ULL;
	};
}