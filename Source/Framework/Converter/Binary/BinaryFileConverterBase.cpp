#include "BinaryFileConverterBase.h"

FWK::Converter::BinaryFileConverterBase::BinaryFileConverterBase() : 
	m_mappedData(nullptr),

	m_fileHandle       (INVALID_HANDLE_VALUE),
	m_fileMappingHandle(nullptr),

	m_mappedDataSize(k_emptyMappedDataSize)
{}
FWK::Converter::BinaryFileConverterBase::~BinaryFileConverterBase()
{
	DestroyMemoryMappedFile();
}

bool FWK::Converter::BinaryFileConverterBase::CreateReadMemoryMappedFile(const std::filesystem::path& a_filePath)
{
	// 既に別のファイルを開いていた場合に備えて前のマッピングを破棄する
	DestroyMemoryMappedFile();

	// ファイルを読み込めてるかを確認
	if (!Utility::File::CanLoadFilePath(a_filePath))
	{
		assert(false && "BinaryFileの読み込みに失敗しました。");
		return false;
	}

	// ファイルを開く
	// CreateFileW(開くファイルパス、
	//			   読み込み専用で開く指定、
	//			   他の読み込みアクセスを許可する指定、
	//			   セキュリティ属性、
	//			   既存ファイルのみ開く指定、
	//			   通常ファイルとして扱う指定、
	//			   テンプレートファイルを使用しないためnullptr);

	m_fileHandle = CreateFileW(a_filePath.c_str(),
							   GENERIC_READ,
							   FILE_SHARE_READ,
							   nullptr,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   nullptr);

	// 無効なファイルハンドルならreturn
	if (m_fileHandle == INVALID_HANDLE_VALUE)
	{
		assert				   (false && "BinaryFileの読み込み用ファイルハンドル作成に失敗しました。");
		DestroyMemoryMappedFile();

		return false;
	}

	LARGE_INTEGER l_fileSize = {};

	// GetFileSizeEx(ファイルハンドル、
	//				 ファイルサイズの受取先);

	if (!GetFileSizeEx(m_fileHandle, &l_fileSize))
	{
		assert				   (false && "BinaryFileのファイルサイズ取得に失敗しました。");
		DestroyMemoryMappedFile();

		return false;
	}

	m_mappedDataSize = static_cast<std::uint64_t>(l_fileSize.QuadPart);
	m_isWritable     = k_isReadOnlyMappedFile;


}