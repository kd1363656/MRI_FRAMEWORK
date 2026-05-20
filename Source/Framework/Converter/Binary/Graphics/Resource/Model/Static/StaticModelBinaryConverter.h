#pragma once

namespace FWK::Converter
{
	class StaticModelBinaryConverter final : public BinaryFileConverterBase
	{
	private:

		struct StaticModelAssetHeader final
		{
			std::uint16_t m_version    = k_staticModelAssetVersion;
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
		};

	public:

		 StaticModelBinaryConverter()		   = default;
		~StaticModelBinaryConverter() override = default;

		bool LoadStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath);
		bool SaveStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath);

	private:

		template <typename Type>
		void ReadBinaryData(const std::uint64_t& a_readDataCount,
							const std::uint8_t*  a_readData,
							      std::uint64_t& a_readOffset,
								  Type*          a_destinationData) const
		{
			const auto l_readDataSize = sizeof(Type) * a_readDataCount;

			if (l_readDataSize == k_emptyReadDataSize) { return; }

			if (!a_destinationData)
			{
				assert(false && "読み込み先データがnullptrです。");
				return;
			}

			if (!a_readData)
			{
				assert(false && "読み込み元データがnullptrです。");
				return;
			}

			std::memcpy(a_destinationData, a_readData + a_readOffset, l_readDataSize);

			a_readOffset += l_readDataSize;
		}

		template <typename Type>
		void WriteBinaryData(const std::uint64_t& a_writeDataCount,
							 const Type*		  a_sourceData,
								   std::uint64_t& a_writeOffset,
								   std::uint8_t* a_writeData) const
		{
			const auto l_writeDataSize = sizeof(Type) * a_writeDataCount;

			if (l_writeDataSize == k_emptyWriteDataSize) { return; }

			if (!a_sourceData)
			{
				assert(false && "書き込み元データがnullptrです。");
				return;
			}

			if (!a_writeData)
			{
				assert(false && "書き込み先データがnullptrです。");
				return;
			}

			std::memcpy(a_writeData + a_writeOffset, a_sourceData, l_writeDataSize);

			a_writeOffset += l_writeDataSize;
		}

		void ReadWStringBinaryData(const std::uint64_t& a_stringBinaryFileSize,
								   const std::uint8_t*  a_readData,
										 std::wstring&  a_string,
										 std::uint64_t& a_readOffset) const;

		void WriteWStringBinaryData(const std::wstring&  a_string, std::uint64_t& a_writeOffset, std::uint8_t* a_writeData) const;

		std::uint64_t CalculateStaticModelAssetFileSize(const Struct::ModelData& a_modelData) const;
		std::uint64_t CalculateWStringBinaryFileSize   (const std::wstring&		 a_string)    const;

		template <typename Type>
		std::uint64_t CalculateBinaryDataSize(const std::uint64_t& a_dataCount) const
		{
			return sizeof(Type) * a_dataCount;
		}

		static constexpr std::uint64_t k_emptyStaticModelAssetFileSize = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelMeshCount     = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelVertexCount   = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelIndexCount    = 0ULL;

		static constexpr std::uint64_t k_emptyStaticModelMeshletCount			= 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelUniqueVertexIndexCount = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelPrimitiveIndexCount    = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelMeshletBoundsCount     = 0ULL;

		static constexpr std::uint64_t k_emptyTextureFilePathSize = 0ULL;

		static constexpr std::uint64_t k_emptyReadDataSize  = 0ULL;
		static constexpr std::uint64_t k_emptyWriteDataSize = 0ULL;

		static constexpr std::uint64_t k_initialReadOffset  = 0ULL;
		static constexpr std::uint64_t k_initialWriteOffset = 0ULL;

		static constexpr std::uint64_t k_singleBinaryElementCount = 1ULL;

		// 'S' = 0x53, 'T' = 0x54のため、0x5354で"ST"を表す
		static constexpr std::uint16_t k_staticModelAssetTypeID = 0x5354U;

		// ※ 注意 : Assetとして保存する構造体が変化したらバージョンを上げる
		static constexpr std::uint16_t k_staticModelAssetVersion = 4U;
	};
}