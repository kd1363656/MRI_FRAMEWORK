#pragma once

namespace FWK::Converter
{
	class BinaryFileConverterBase
	{
	public:

				 BinaryFileConverterBase();
		virtual ~BinaryFileConverterBase();

	protected:

		bool CreateReadMemoryMappedFile (const std::filesystem::path& a_filePath);
		bool CreateWriteMemoryMappedFile(const std::filesystem::path& a_filePath, const std::uint64_t& a_fileSize);

		void DestroyMemoryMappedFile();

		const auto& GetREFMappedDataSize() const { return m_mappedDataSize; }

		const std::uint8_t* GetPTRMappedData() const { return m_mappedData; }

		std::uint8_t* GetMutablePTRMappedData() { return m_mappedData; }

	private:

		static constexpr std::uint64_t k_emptyMappedDataSize = 0ULL;
		static constexpr std::uint64_t k_emptyWriteFileSize  = 0ULL;

		static constexpr SIZE_T k_mapEntireFileSize   = 0ULL;
		static constexpr SIZE_T k_flushEntireViewSize = 0ULL;
		
		static constexpr DWORD k_fileSizeHigh				   = 0UL;
		static constexpr DWORD k_mappingMaxSizeHighUseFileSize = 0UL;
		static constexpr DWORD k_viewFileOffsetHighFromBegin   = 0UL;
		static constexpr DWORD k_viewFileOffsetLowFromBegin    = 0UL;
		static constexpr DWORD k_noFileShaderMode			   = 0UL;

		static constexpr bool k_isInitialWritable    = false;
		static constexpr bool k_isReadOnlyMappedFile = false;
		static constexpr bool k_isWriteMappedFile    = true;

		std::uint8_t* m_mappedData;

		HANDLE m_fileHandle;
		HANDLE m_fileMappingHandle;

		std::uint64_t m_mappedDataSize;

		bool m_isWritable = k_isInitialWritable;
	};
}