#pragma once

namespace FWK::Graphics
{
	class UploadSystem;
}

namespace FWK::Graphics
{
	class TextureSystem final
	{
	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Device&                            a_device,
					const GPUMemoryAllocator&                a_gpuMemoryAllocator,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  UploadSystem&                      a_uploadSystem);

		Struct::TextureLoadResult LoadTextureForBatchUpload(const Device&			                 a_device, 
													        const GPUMemoryAllocator&                a_gpuMemoryAllocator,
													        const std::filesystem::path&			 a_filePath,
																  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		void LoadPendingTexturesAndWait(UploadSystem& a_uploadSystem);

		void ReleaseCompletedUnusedTexture(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		nlohmann::json Serialize() const;

		bool AddTextureReference    (const TypeAlias::StorageID a_storageID);
		bool ReleaseTextureReference(const DirectCommandQueue&  a_directCommandQueue, const TypeAlias::StorageID a_storageID);

		std::weak_ptr<Struct::TextureRecord> FindVALDefaultTextureRecord(const Enum::DefaultTextureType a_defaultTextureType) const;
		std::weak_ptr<Struct::TextureRecord> FindVALTextureRecord       (const TypeAlias::StorageID     a_storageID) const;

		const auto& GetREFTextureStorage() const { return m_textureStorage; }

		auto& GetMutableREFTextureStorage() { return m_textureStorage; }


	private:

		bool TextureCopyBatch(UploadSystem& a_uploadSystem);

		TypeAlias::PendingTextureBatchUploadRecordMap m_pendingTextureBatchUploadRecordMap = {};

		std::array<std::shared_ptr<Struct::TextureRecord>, Constant::k_defaultTextureCount> m_defaultTextureRecordList = {};
		
		AssetStorage<Struct::TextureRecord> m_textureStorage = {};
		
		TextureLoader			        m_textureLoader				      = {};
		TextureBatchUploadRecordBuilder m_textureBatchUploadRecordBuilder = {};
		DefaultTextureBuilder			m_defaultTextureBuilder			  = {};

		Converter::TextureSystemJsonConverter m_textureSystemJsonConverter = {};
	};
}