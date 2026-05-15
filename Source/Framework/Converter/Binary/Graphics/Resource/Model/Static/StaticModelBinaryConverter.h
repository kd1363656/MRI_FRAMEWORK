#pragma once

namespace FWK::Converter
{
	class StaticModelBinaryConverter final : public BinaryFileConverterBase
	{
	private:

		struct StaticModelAssetHeader final
		{
			std::uint64_t m_fileSize    = k_emptyStaticModelAssetFileSize;
			std::uint64_t m_meshCount   = k_emptyStaticModelMeshCount;
			std::uint16_t m_assetTypeID = k_staticModelAssetTypeID;
			std::uint16_t m_version     = k_staticModelAssetVersion;
		};

		struct StaticModelAssetMeshHeader final
		{
			std::uint64_t m_vertexCount              = k_emptyStaticModelVertexCount;
			std::uint64_t m_indexCount               = k_emptyStaticModelIndexCount;
			std::uint64_t m_baseColorTexturePathSize = k_emptyTextureFilePathSize;
			std::uint64_t m_normalTexturePathSize    = k_emptyTextureFilePathSize;
			std::uint64_t m_roughnessTexturePathSize = k_emptyTextureFilePathSize;
			std::uint64_t m_metallicTexturePathSize  = k_emptyTextureFilePathSize;
		};

	public:

		 StaticModelBinaryConverter()		   = default;
		~StaticModelBinaryConverter() override = default;

		bool LoadStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath);
		bool SaveStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath);

	private:

		void ReadBinaryData(const std::uint64_t& a_readDataSize,
							const std::uint8_t*  a_readData,
								  std::uint64_t& a_readOffset,
								  void*			 a_destinationData) const;

		void ReadWStringBinaryData(const std::uint64_t& a_stringBinaryFileSize,
								   const std::uint8_t*  a_readData,
										 std::wstring&  a_string,
										 std::uint64_t& a_readOffset) const;

		void WriteBinaryData(const std::uint64_t& a_writeDataSize,
							 const void*          a_sourceData, 
								   std::uint64_t& a_writeOffset, 
								   std::uint8_t*  a_writeData) const;
		
		void WriteWStringBinaryData(const std::wstring&  a_string, std::uint64_t& a_writeOffset, std::uint8_t* a_writeData) const;

		std::uint64_t CalculateStaticModelAssetFileSize(const Struct::ModelData& a_modelData) const;
		std::uint64_t CalculateWStringBinaryFileSize   (const std::wstring&		 a_string)    const;

		static constexpr std::uint64_t k_emptyStaticModelAssetFileSize = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelMeshCount     = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelVertexCount   = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelIndexCount    = 0ULL;

		static constexpr std::uint64_t k_emptyTextureFilePathSize = 0ULL;

		static constexpr std::uint64_t k_emptyReadDataSize  = 0ULL;
		static constexpr std::uint64_t k_emptyWriteDataSize = 0ULL;

		static constexpr std::uint64_t k_initialReadOffset  = 0ULL;
		static constexpr std::uint64_t k_initialWriteOffset = 0ULL;

		// 'S' = 0x53, 'T' = 0x54のため、0x5354で"ST"を表す
		static constexpr std::uint16_t k_staticModelAssetTypeID = 0x5354U;

		// ※ 注意 : Assetとして保存する構造体が編かしたらバージョンを上げる
		static constexpr std::uint16_t k_staticModelAssetVersion = 1U;
	};
}