#pragma once

namespace FWK::Converter
{
	class BinaryFileConverterBase final
	{
	public:

				 BinaryFileConverterBase() = default;
		virtual ~BinaryFileConverterBase() = default;

	protected:

		bool LoadBinaryFile(std::vector<std::uint8_t>& a_binaryDataList, const std::filesystem::path& a_filePath) const;

		bool SaveBinaryFile(const std::vector<std::uint8_t>& a_binaryDataList, const std::filesystem::path& a_filePath) const;

		/*template <typename Type>
		void WriteBinaryValue(const Type& a_value, std::vector<std::uint8_t>& a_binaryDataList) const;

		template <typename Type>
		bool ReadBinaryValue(const std::vector<std::uint8_t>& a_bainaryDataList, Type& a_value, std::size_t& a_readOffset) const;

		template <typename Type>
		void WriteBinaryArray(const std::vector<Type>& a_valueList, std::vector<std::uint8_t>& a_binaryDataList) const;

		template <typename Type>
		bool ReadBinaryArray(const std::vector<std::uint8_t>& a_binaryDataList, std::vector<Type>& a_valueList, std::size_t& a_readOffset) const;*/

	private:

		static constexpr std::uint64_t k_emptyBinaryFileSize = 0ULL;
	};
}