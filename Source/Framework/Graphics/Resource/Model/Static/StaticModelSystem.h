#pragma once

namespace FWK::Graphics
{
	class UploadSystem;
}

namespace FWK::Graphics
{
	class StaticModelSystem final
	{
	public:

		 StaticModelSystem() = default;
		~StaticModelSystem() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     ();

		Struct::StaticModelResult LoadStaticModelForBatchUpload(const Device&			                 a_device, 
																const GPUMemoryAllocator&                a_gpuMemoryAllocator, 
																const std::filesystem::path&             a_filePath, 
																	  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		void LoadPendingStaticModelAndWait(UploadSystem& a_uploadSystem);
		
		nlohmann::json Serialize() const;

		bool AddStaticModelReference    (const std::weak_ptr<Graphics::StaticModelRecord>& a_staticModelRecord);
		bool ReleaseStaticModelReference(const std::weak_ptr<Graphics::StaticModelRecord>& a_staticModelRecord, const DirectCommandQueue& a_directCommandQueue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue);

		std::weak_ptr<Graphics::StaticModelRecord> FindVALStaticModelRecord(const std::wstring& a_filePath) const;

		const auto& GetREFStaticModelStorage() const { return m_staticModelStorage; }

		auto& GetMutableREFStaticModelStorage() { return m_staticModelStorage; }

	private:

		bool					 CreateStaticModelAssetFromFBX(const std::filesystem::path& a_fbxFilePath,	 const std::filesystem::path& a_assetFilePath,         Graphics::StaticModelRecord& a_staticModelRecord);
		std::shared_ptr<Texture> CreateMaterialTexture		  (const std::filesystem::path& a_modelFilePath, const std::wstring&		  a_textureFilePath, const Enum::DefaultTextureType     a_defaultTextureType) const;

		bool LoadStaticModel(const std::filesystem::path& a_filePath, Graphics::StaticModelRecord& a_staticModelRecord);

		bool LoadStaticModelAsset(const std::filesystem::path& a_assetFilePath, Graphics::StaticModelRecord& a_staticModelRecord);

		bool CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const;

		bool StaticModelCopyBatch(UploadSystem& a_uploadSystem);

		TypeAlias::PendingStaticModelBatchUploadRecordMap m_pendingStaticModelBatchUploadRecordMap = {};

		AssetStorage<Graphics::StaticModelRecord> m_staticModelStorage = {};

		StaticModelFBXLoader                m_staticModelFBXLoader                = {};
		StaticModelMeshOptimizer            m_staticModelMeshOptimizer            = {};
		StaticModelMeshletBuilder           m_staticModelMeshletBuilder           = {};
		StaticModelBatchUploadRecordBuilder m_staticModelBatchUploadRecordBuilder = {};

		Converter::StaticModelSystemJsonConverter m_staticModelSystemJsonConverter = {};
		Converter::StaticModelBinaryConverter     m_staticModelBinaryConverter     = {};
	};
}