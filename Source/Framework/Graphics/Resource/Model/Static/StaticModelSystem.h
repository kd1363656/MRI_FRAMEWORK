#pragma once

namespace FWK::Graphics
{
	class StaticModelSystem final
	{
	public:

		 StaticModelSystem() = default;
		~StaticModelSystem() = default;

		bool LoadStaticModel(Struct::ModelData& a_modelData, const std::filesystem::path& a_fbxFilePath);		

	private:

		bool CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const;

		bool LoadStaticModelAsset(Struct::ModelData& a_modelData, const std::filesystem::path& a_assetFilePath);

		bool CreateStaticModelAssetFromFBX(Struct::ModelData& a_modelData, const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath);

		StaticModelFBXLoader     m_staticModelFBXLoader     = {};
		StaticModelMeshOptimizer m_staticModelMeshOptimizer = {};

		Converter::StaticModelBinaryConverter m_staticModelBinaryConverter = {};
	};
}