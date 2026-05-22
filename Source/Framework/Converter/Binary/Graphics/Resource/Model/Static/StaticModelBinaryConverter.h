#pragma once

namespace FWK::Converter
{
	class StaticModelBinaryConverter final : public BinaryFileConverterBase
	{
	private:

		struct StaticModelAssetHeader final
		{
			std::uint16_t m_version     = k_staticModelAssetVersion;
			std::uint64_t m_fileSize    = k_emptyStaticModelAssetFileSize;
			std::uint64_t m_meshCount   = k_emptyStaticModelMeshCount;
			std::uint16_t m_assetTypeID = k_staticModelAssetTypeID;
		};

		struct StaticModelAssetMeshHeader final
		{
			std::uint64_t m_vertexCount = k_emptyStaticModelVertexCount;
			std::uint64_t m_indexCount  = k_emptyStaticModelIndexCount;

			std::uint64_t m_meshletCount		   = k_emptyStaticModelMeshletCount;
			std::uint64_t m_uniqueVertexIndexCount = k_emptyStaticModelUniqueVertexIndexCount;
			std::uint64_t m_primitiveIndexCount	   = k_emptyStaticModelPrimitiveIndexCount;
			std::uint64_t m_meshletBoundsCount     = k_emptyStaticModelMeshletBoundsCount;

			std::uint64_t m_baseColorTexturePathSize = k_emptyTextureFilePathSize;
			std::uint64_t m_normalTexturePathSize    = k_emptyTextureFilePathSize;
		};

	public:

		 StaticModelBinaryConverter()		   = default;
		~StaticModelBinaryConverter() override = default;

		bool LoadStaticModelAsset(Struct::StaticModelRecord& a_staticModelRecord, const std::filesystem::path& a_filePath);

		bool SaveStaticModelAsset(const Struct::StaticModelRecord& a_staticModelRecord, const std::filesystem::path& a_filePath);

	private:

		std::uint64_t CalculateStaticModelAssetFileSize(const Struct::ModelData& a_modelData) const;
		
		static constexpr std::uint64_t k_emptyStaticModelAssetFileSize = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelMeshCount     = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelVertexCount   = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelIndexCount    = 0ULL;

		static constexpr std::uint64_t k_emptyStaticModelMeshletCount			= 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelUniqueVertexIndexCount = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelPrimitiveIndexCount    = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelMeshletBoundsCount     = 0ULL;

		static constexpr std::uint64_t k_emptyTextureFilePathSize = 0ULL;

		static constexpr std::uint64_t k_initialReadOffset  = 0ULL;
		static constexpr std::uint64_t k_initialWriteOffset = 0ULL;

		static constexpr std::uint64_t k_singleBinaryElementCount = 1ULL;

		// 'S' = 0x53, 'T' = 0x54のため、0x5354で"ST"を表す
		static constexpr std::uint16_t k_staticModelAssetTypeID = 0x5354U;

		// ※ 注意 : Assetとして保存する構造体が変化したらバージョンを上げる
		static constexpr std::uint16_t k_staticModelAssetVersion = 6U;
	};
}