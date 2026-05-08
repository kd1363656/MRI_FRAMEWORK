#pragma once

namespace FWK::Converter
{
	class BinaryFileConverterBase
	{
	public:
		
				 BinaryFileConverterBase();
		virtual ~BinaryFileConverterBase();

		bool CreateReadMemoryMappedFile (const std::filesystem::path& a_filePath);
		bool CreateWriteMemoryMappedFile(const std::filesystem::path& a_filePath, const std::uint64_t& a_fileSize);

		void DestroyMemoryMappedFile();

		const auto& GetREFMappedDataSize() const { return m_mappedDataSize; }

		const std::uint8_t* GetPTRMappedData() const { return m_mappedData; }

		std::uint8_t* GetMutablePTRMappedData() { return m_mappedData; }

	private:

		static constexpr std::uint64_t k_initialMappedDataSize = 0ULL;
		static constexpr std::uint64_t k_emptyWriteFileSize    = 0ULL;
		static constexpr SIZE_T		   k_mapEntireFileSize     = 0ULL;
		static constexpr SIZE_T		   k_flushEntireViewSize   = 0ULL;

		static constexpr DWORD k_fileOffsetHighFromBegin = 0U;
		static constexpr DWORD k_fileOffsetLowFromBegin	 = 0U;
		static constexpr DWORD k_noFileShareMode		 = 0U;

		static constexpr bool k_initialIsWritable    = false;
		static constexpr bool k_isReadOnlyMappedFile = false;
		static constexpr bool k_isWriteMappedFile	 = true;

		HANDLE m_fileHandle		   = INVALID_HANDLE_VALUE;
		HANDLE m_fileMappingHandle = nullptr;

		std::uint8_t* m_mappedData = nullptr;

		std::uint64_t m_mappedDataSize = k_initialMappedDataSize;

		bool m_isWritable = false;
	};
}