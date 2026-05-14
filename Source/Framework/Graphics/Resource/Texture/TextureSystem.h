#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    ();

		Struct::TextureLoadResult LoadTextureForBatchUpload(const Device&			                 a_device, 
													        const GPUMemoryAllocator&                a_gpuMemoryAllocator,
													        const std::filesystem::path&			 a_filePath,
																  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		void LoadPendingTexturesAndWait(UploadSystem& a_uploadSystem);

		void ReleaseCompletedUnusedTexture(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		nlohmann::json Serialize() const;

		bool AddTextureReference    (const TypeAlias::StorageID a_storageID);
		bool ReleaseTextureReference(const DirectCommandQueue&  a_directCommandQueue, const TypeAlias::StorageID a_storageID);

		void SetStorageIDAllocatorCapacity(const TypeAlias::StorageID a_set) { m_storageIDAllocatorCapacity = a_set; }

		std::weak_ptr<Struct::TextureRecord> FindVALTextureRecord(const TypeAlias::StorageID a_storageID) const;

		const auto& GetREFTextureStorage() const { return m_textureStorage; }

		auto& GetMutableREFTextureStorage() { return m_textureStorage; }

		auto GetVALStorageIDAllocatorCapacity() const { return m_storageIDAllocatorCapacity; }

	private:

		bool TextureCopyBatch(UploadSystem& a_uploadSystem);

		TypeAlias::PendingTextureBatchUploadRecordMap m_pendingTextureBatchUploadRecordMap = {};

		AssetStorage<Struct::TextureRecord> m_textureStorage = {};
		
		TextureLoader			        m_textureLoader				      = {};
		TextureBatchUploadRecordBuilder m_textureBatchUploadRecordBuilder = {};

		Converter::TextureSystemJsonConverter m_textureSystemJsonConverter = {};

		TypeAlias::StorageID m_storageIDAllocatorCapacity = Constant::k_defaultCreateStorageIDCapacity;
	};
}