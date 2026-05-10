#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	private:

		using TexturePathStorageIDMap = std::unordered_map<std::wstring,	     TypeAlias::StorageID, Struct::WStringHash, std::equal_to<>>;
		using TextureRecordMap		  = std::unordered_map<TypeAlias::StorageID, Struct::TextureRecord>;
		
	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    ();

		TypeAlias::StorageID LoadTextureForBatchUpload(const Device&			                a_device, 
													   const GPUMemoryAllocator&                a_gpuMemoryAllocator,
													   const std::filesystem::path&				a_filePath,
															 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		void LoadPendingTexturesAndWait(UploadSystem& a_uploadSystem);

		void ReleaseCompletedUnusedTexture(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		nlohmann::json Serialize() const;

		bool AddTextureReference    (const TypeAlias::StorageID a_storageID);
		bool ReleaseTextureReference(const DirectCommandQueue&  a_directCommandQueue, const TypeAlias::StorageID a_storageID);

		void SetStorageIDAllocatorCapacity(const TypeAlias::StorageID a_set) { m_storageIDAllocatorCapacity = a_set; }

		const Struct::TextureRecord* FindPTRTextureRecord(const TypeAlias::StorageID a_storageID) const;

		Struct::TextureRecord* FindMutablePTRTextureRecord(const TypeAlias::StorageID a_storageID);

		auto GetVALStorageIDAllocatorCapacity() const { return m_storageIDAllocatorCapacity; }

	private:

		bool TextureCopyBatch(UploadSystem& a_uploadSystem);

		TexturePathStorageIDMap m_texturePathStorageIDMap = {};
		TextureRecordMap		m_textureRecordMap		  = {};

		TypeAlias::PendingTextureBatchUploadRecordMap m_pendingTextureBatchUploadRecordMap = {};

		StorageIDAllocator		        m_storageIDAllocator		      = {};
		TextureLoader			        m_textureLoader				      = {};
		TextureBatchUploadRecordBuilder m_textureBatchUploadRecordBuilder = {};

		Converter::TextureSystemJsonConverter m_textureSystemJsonConverter = {};

		TypeAlias::StorageID m_storageIDAllocatorCapacity = Constant::k_defaultCreateStorageIDCapacity;
	};
}