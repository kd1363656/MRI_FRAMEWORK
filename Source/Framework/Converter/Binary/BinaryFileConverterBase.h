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

		auto* GetMutablePTRMappedData() { return m_mappedData; }

	private:

		void DestroyMemoryMappedFile();

		static constexpr SIZE_T k_mapEntireFileSize   = 0ULL;
		static constexpr SIZE_T k_flushEntireViewSize = 0ULL;

		static constexpr std::uint64_t k_initialMappedDataSize = 0ULL;
		static constexpr std::uint64_t k_emptyWriteFileSize    = 0ULL;

		static constexpr DWORD k_mappingMAXSizeHighUseFileSize = 0U;
		static constexpr DWORD k_mappingMAXSizeLowUseFileSize  = 0U;
		static constexpr DWORD k_viewFileOffsetHighFromBegin   = 0U;
		static constexpr DWORD k_viewFileOffsetLowFromBegin    = 0U;
		static constexpr DWORD k_noFileShareMode			   = 0U;
		
		static constexpr bool k_isReadOnlyMappedFile  = false;
		static constexpr bool k_isWriteableMappedFile = true;
		
		HANDLE m_fileHandle;
		HANDLE m_fileMappingHandle;

		std::uint8_t* m_mappedData;

		std::uint64_t m_mappedDataSize;

		bool m_isWritable;
	};
}