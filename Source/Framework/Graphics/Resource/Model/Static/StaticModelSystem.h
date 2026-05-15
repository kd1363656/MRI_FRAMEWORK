#pragma once

namespace FWK::Graphics
{
	class StaticModelSystem final
	{
	public:

		 StaticModelSystem() = default;
		~StaticModelSystem() = default;

		bool LoadStaticModel(const std::shared_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_fbxFilePath);		

	private:

		bool CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const;

		bool LoadStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_assetFilePath);

		bool CreateStaticModelAssetFromFBX(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath);

		StaticModelFBXLoader      m_staticModelFBXLoader      = {};
		StaticModelMeshOptimizer  m_staticModelMeshOptimizer  = {};
		StaticModelMeshletBuilder m_staticModelMeshletBuilder = {};

		Converter::StaticModelBinaryConverter m_staticModelBinaryConverter = {};
	};
}