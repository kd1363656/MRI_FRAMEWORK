#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	private:

		using TexturePathMap   = std::unordered_map<std::wstring,			TypeAlias::TextureID, Struct::WStringHash, std::equal_to<>>;
		using TextureRecordMap = std::unordered_map<TypeAlias::TextureID, Struct::TextureRecord>;
		
	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    ();

		TypeAlias::TextureID LoadTextureForBatchUpload(const Device&			                a_device, 
													   const GPUMemoryAllocator&                a_gpuMemoryAllocator,
													   const std::filesystem::path&				a_filePath,
															 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		void LoadPendingTexturesAndWait(UploadSystem& a_uploadSystem);

		void ReleaseCompletedUnusedTexture(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		nlohmann::json Serialize() const;

		bool AddTextureReference    (const TypeAlias::TextureID a_textureID);
		bool ReleaseTextureReference(const DirectCommandQueue&  a_directCommandQueue, const TypeAlias::TextureID a_textureID);

		void SetTextureIDAllocatorCapacity(const TypeAlias::TextureID a_set) { m_textureIDAllocatorCapacity = a_set; }

		const Struct::TextureRecord* FindPTRTextureRecord(const TypeAlias::TextureID a_textureID) const;

		Struct::TextureRecord* FindMutablePTRTextureRecord(const TypeAlias::TextureID a_textureID);

		auto GetVALTextureIDAllocatorCapacity() const { return m_textureIDAllocatorCapacity; }

	private:

		bool TextureCopyBatch(UploadSystem& a_uploadSystem);

		TexturePathMap   m_texturePathMap   = {};
		TextureRecordMap m_textureRecordMap = {};

		TypeAlias::PendingTextureBatchUploadRecordMap m_pendingTextureBatchUploadRecordMap = {};

		TextureIDAllocator		        m_textureIDAllocator		      = {};
		TextureLoader			        m_textureLoader				      = {};
		TextureBatchUploadRecordBuilder m_textureBatchUploadRecordBuilder = {};

		JsonConverter::TextureSystemJsonConverter m_textureSystemJsonConverter = {};

		TypeAlias::TextureID m_textureIDAllocatorCapacity = Constant::k_defaultCreateTextureIDCapacity;
	};
}