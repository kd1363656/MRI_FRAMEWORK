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

		nlohmann::json Serialize() const;

		bool AddTextureReference    (const std::weak_ptr<Graphics::TextureRecord>& a_textureRecord);
		bool ReleaseTextureReference(const std::weak_ptr<Graphics::TextureRecord>& a_textureRecord, const DirectCommandQueue& a_directCommandQueue, DeferredResourceReleaseQueue& a_deferredReleaseQueue);

		std::weak_ptr<Graphics::TextureRecord> FindVALDefaultTextureRecord(const Enum::DefaultTextureType a_defaultTextureType) const;
		std::weak_ptr<Graphics::TextureRecord> FindVALTextureRecord       (const std::wstring&		      a_filePath) const;

		const auto& GetREFTextureStorage() const { return m_textureStorage; }

		auto& GetMutableREFTextureStorage() { return m_textureStorage; }


	private:

		bool TextureCopyBatch(UploadSystem& a_uploadSystem);

		static constexpr std::size_t k_defaultTextureCount = static_cast<std::size_t>(Enum::DefaultTextureType::Count);

		TypeAlias::PendingTextureBatchUploadRecordMap m_pendingTextureBatchUploadRecordMap = {};

		std::array<std::shared_ptr<Graphics::TextureRecord>, k_defaultTextureCount> m_defaultTextureRecordList = {};
		
		AssetStorage<Graphics::TextureRecord> m_textureStorage = {};
		
		TextureLoader			        m_textureLoader				      = {};
		TextureBatchUploadRecordBuilder m_textureBatchUploadRecordBuilder = {};
		DefaultTextureBuilder			m_defaultTextureBuilder			  = {};

		Converter::TextureSystemJsonConverter m_textureSystemJsonConverter = {};
	};
}