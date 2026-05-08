#pragma once

namespace FWK::Converter
{
	class ModelBinaryConverter final : public BinaryFileConverterBase
	{
	private:

		struct ModelAssetHeader final
		{
			std::uint64_t m_fileSize  = k_initialFileSize;
			std::uint32_t m_magic     = k_modelAssetMagic;
			std::uint32_t m_version   = k_modelAssetVersion;
			std::uint32_t m_meshCount = k_initialMeshCount;
		};

		struct ModelAssetMeshHeader final
		{
			std::uint64_t m_vertexCount            = k_initialElementCount;
			std::uint64_t m_indexCount             = k_initialElementCount;
			std::uint64_t m_meshletCount           = k_initialElementCount;
			std::uint64_t m_uniqueVertexIndexCount = k_initialElementCount;
			std::uint64_t m_primitiveIndexCount    = k_initialElementCount;
		};


	public:

		 ModelBinaryConverter()			 = default;
		~ModelBinaryConverter() override = default;

		bool SaveStaticModelAsset(const std::vector<Struct::ModelMeshletData>& a_modelMeshletDataList, const Struct::StaticModelData& a_staticModelData, const std::filesystem::path& a_filePath);

	private:

		std::uint64_t CalculateStaticModelAssetFileSize(const std::vector<Struct::ModelMeshletData>& a_modelMeshletDataList, const Struct::StaticModelData& a_staticModelData) const;

		bool WriteBinaryData(const std::uint64_t& a_writeSize,
							 const std::uint64_t& a_fileSize, 
						     const void*          a_sourcePTR,
						     std::uint64_t&		  a_writeOffset,
						     std::uint8_t*        a_basePTR) const;
		
		// ModelAssetファイルかどうかを判定するための識別値
		// リトルエンディアン環境では"FWKM"として保存される
		static constexpr std::uint32_t k_modelAssetMagic = 0x4D4B5746U;

		static constexpr std::uint32_t k_modelAssetVersion = 1U;

		static constexpr std::uint32_t k_initialMeshCount = 0U;

		static constexpr std::uint64_t k_initialFileSize     = 0ULL;
		static constexpr std::uint64_t k_initialElementCount = 0ULL;
		static constexpr std::uint64_t k_emptyFileSize		 = 0ULL;
		static constexpr std::uint64_t k_emptyWriteSize		 = 0ULL;
		static constexpr std::uint64_t k_initialWriteOffset  = 0ULL;

		static constexpr std::size_t k_initialMeshIndex = 0ULL;
	};
}