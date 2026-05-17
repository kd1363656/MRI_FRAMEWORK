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
		
		void ReleaseCompletedUnusedStaticModel(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		nlohmann::json Serialize() const;

		bool AddStaticModelReference    (const TypeAlias::StorageID a_storageID);
		bool ReleaseStaticModelReference(const DirectCommandQueue&  a_directCommandQueue, const TypeAlias::StorageID a_storageID);

		std::weak_ptr<Struct::StaticModelRecord> FindVALStaticModelRecord(const TypeAlias::StorageID a_storageID) const;

		const auto& GetREFStaticModelStorage() const { return m_staticModelStorage; }

		auto& GetMutableREFStaticModelStorage() { return m_staticModelStorage; }

	private:

		bool LoadStaticModel(const std::shared_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_fbxFilePath);

		bool CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const;

		bool LoadStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_assetFilePath);

		bool StaticModelCopyBatch(UploadSystem& a_uploadSystem);

		bool CreateStaticModelAssetFromFBX(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath);

		TypeAlias::PendingStaticModelBatchUploadRecordMap m_pendingStaticModelBatchUploadRecordMap = {};

		AssetStorage<Struct::StaticModelRecord> m_staticModelStorage = {};

		StaticModelFBXLoader                m_staticModelFBXLoader                = {};
		StaticModelMeshOptimizer            m_staticModelMeshOptimizer            = {};
		StaticModelMeshletBuilder           m_staticModelMeshletBuilder           = {};
		StaticModelBatchUploadRecordBuilder m_staticModelBatchUploadRecordBuilder = {};

		Converter::StaticModelSystemJsonConverter m_staticModelSystemJsonConverter = {};
		Converter::StaticModelBinaryConverter     m_staticModelBinaryConverter     = {};
	};
}