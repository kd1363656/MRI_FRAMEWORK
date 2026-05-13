#pragma once

namespace FWK::Converter
{
	class StaticModelBinaryConverter final : public BinaryFileConverterBase
	{
	public:

		 StaticModelBinaryConverter()		   = default;
		~StaticModelBinaryConverter() override = default;

		bool SaveStaticModelAsset(const std::filesystem::path& a_filePath, const Struct::ModelData& a_modelData);
		bool LoadStaticModelAsset(const std::filesystem::path& a_filePath,		 Struct::ModelData& a_modelData);

	private:


		std::uint64_t CalculateStaticModelAssetFileSize(const Struct::ModelData& a_modelData) const;
		std::uint64_t CalculateWStringBinaryFileSize   (const std::wstring&		 a_string)    const;

		static constexpr std::uint64_t k_emptyStaticModelAssetFileSize = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelMeshCount     = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelVertexCount   = 0ULL;
		static constexpr std::uint64_t k_emptyStaticModelIndexCount    = 0ULL;
		static constexpr std::uint64_t k_emptyTextureFilePathSize      = 0ULL;

		// 'S' = 0x53, "T" = 0x54のため、0x5354で"ST"を表す
		static constexpr std::uint16_t k_staticModelAssetTypeID = 0x5354U;

		static constexpr std::uint16_t k_staticModelAssetVersion = 1U;
	};
}