#pragma once

namespace FWK::Graphics
{
	class StaticModelSystem final
	{
	public:

		 StaticModelSystem() = default;
		~StaticModelSystem() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     ();

		Struct::StaticModelResult LoadStaticModelForBatchUpload(const std::filesystem::path& a_filePath);

		bool LoadStaticModel(const std::shared_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_fbxFilePath);		

		nlohmann::json Serialize() const;

		bool AddStaticModelReference    (const TypeAlias::StorageID a_storageID);
		bool ReleaseStaticModelReference(const DirectCommandQueue&  a_directCommandQueue, const TypeAlias::StorageID a_storageID);

		std::weak_ptr<Struct::StaticModelRecord> FindVALStaticModelRecord(const TypeAlias::StorageID a_storageID);

		const auto& GetREFStaticModelStorage() const { return m_staticModelStorage; }

		auto& GetMutableREFStaticModelStorage() { return m_staticModelStorage; }

	private:

		bool CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const;

		bool LoadStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_assetFilePath);

		bool CreateStaticModelAssetFromFBX(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath);

		AssetStorage<Struct::StaticModelRecord> m_staticModelStorage = {};

		StaticModelFBXLoader      m_staticModelFBXLoader      = {};
		StaticModelMeshOptimizer  m_staticModelMeshOptimizer  = {};
		StaticModelMeshletBuilder m_staticModelMeshletBuilder = {};

		Converter::StaticModelSystemJsonConverter m_staticModelSystemJsonConverter = {};
		Converter::StaticModelBinaryConverter     m_staticModelBinaryConverter     = {};
	};
}