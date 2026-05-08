#include "BinaryFileConverterBase.h"

FWK::Converter::BinaryFileConverterBase::BinaryFileConverterBase() : 
	m_fileHandle	   (INVALID_HANDLE_VALUE),
	m_fileMappingHandle(nullptr),

	m_mappedData(nullptr),

	m_mappedDataSize(k_initialMappedDataSize),

	m_isWritable(k_isReadOnlyMappedFile)
{}
FWK::Converter::BinaryFileConverterBase::~BinaryFileConverterBase()
{
	DestroyMemoryMappedFile();
}

bool FWK::Converter::BinaryFileConverterBase::CreateReadMemoryMappedFile(const std::filesystem::path& a_filePath)
{
	// 既に別のファイルを開いていた場合に備えて前のマッピングを破棄する
	DestroyMemoryMappedFile();

	// ファイルを読み込めるかを確認
	if (!Utility::File::CanLoadFilePath(a_filePath))
	{
		assert(false && "BinaryFileの読み込みに失敗しました。");
		return false;
	}

	// ファイルを開く関数
	// CreateFileW(開くファイルパス、
	//			   読み込み専用で開く指定、
	//			   他の読み込みアクセスを指定、
	//			   セキュリティ属性、
	//			   既存ファイルのみ開く指定、
	//			   通常ファイルとして扱う指定、
	//			   テンプレートファイルを使用しないためnullptr)；

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

	// ファイルサイズを取得
	if (!GetFileSizeEx(m_fileHandle, &l_fileSize))
	{
		assert				   (false && "BinaryFileのファイルサイズ取得に失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	m_mappedDataSize = static_cast<std::uint64_t>(l_fileSize.QuadPart);
	m_isWritable	 = k_isReadOnlyMappedFile;

	// ファイルをメモリマップできるようにするための中間オブジェクトを作成
	// CreateFileMappingW(マッピング対象のファイルハンドル、
	//					  セキュリティ属性,
	// 			　		  読み込み専用ページとして作成する指定、
	//					  最大サイズ上位32bit、0ならファイルサイズを使用、
	//					  最大サイズ下位32bit、0ならファイルサイズを使用、
	//					  名前付きマッピング);

	m_fileMappingHandle = CreateFileMappingW(m_fileHandle,
											 nullptr,
											 PAGE_READONLY,
											 k_mappingMAXSizeHighUseFileSize,
											 k_mappingMAXSizeLowUseFileSize,
											 nullptr);

	if (!m_fileMappingHandle)
	{
		assert				   (false && "BinaryFileの読み込み用ファイルマッピング作成に失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	// 実際にファイルをメモリ空間へマップする
	// MapViewOfFile(マッピングオブジェクトのハンドル、
	//				 読み込み専用でビューを作成する指定、
	//				 ファイルオフセット上位32bit、先頭から読むため0、
	//				 ファイルオフセット下位32bit、先頭から読むため0、
	//				 マップするサイズ、0ならファイル全体);
	m_mappedData = static_cast<std::uint8_t*>(MapViewOfFile(m_fileMappingHandle,
															FILE_MAP_READ,
															k_viewFileOffsetHighFromBegin,
															k_viewFileOffsetLowFromBegin,
															k_mapEntireFileSize));

	if (!m_mappedData)
	{
		assert				   (false && "BinaryFileの読み込み用メモリマップに失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	return true;
}
bool FWK::Converter::BinaryFileConverterBase::CreateWriteMemoryMappedFile(const std::filesystem::path& a_filePath, const std::uint64_t& a_fileSize)
{
	DestroyMemoryMappedFile();

	if (a_fileSize == k_emptyWriteFileSize)
	{
		assert(false && "BinaryFileの書き込みサイズが0です。");
		return false;
	}

	if (const auto& l_parentPath = a_filePath.parent_path();
		!l_parentPath.empty())
	{
		std::filesystem::create_directories(l_parentPath);
	}

	// CreateFileW(作成するファイルパス、
	//			   読み込みと書き込みを行う指定、
	//			   他のアクセスを許可しない指定、
	//			   セキュリティ属性、
	//			   常に新規作成または上書きする指定、
	//			   通常ファイルとして扱う指定、
	//			   テンプレートファイルを使用しないためnullptr);

	m_fileHandle = CreateFileW(a_filePath.c_str(),
							   GENERIC_READ | GENERIC_WRITE,
							   k_noFileShareMode,
							   nullptr,
							   CREATE_ALWAYS,
							   FILE_ATTRIBUTE_NORMAL,
							   nullptr);

	if (m_fileHandle == INVALID_HANDLE_VALUE)
	{
		assert				   (false && "BinaryFileの書き込み用ファイルハンドル作成に失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	LARGE_INTEGER l_fileSize = {};

	l_fileSize.QuadPart = static_cast<LONGLONG>(a_fileSize);

	// SetFilePointerEx(ファイルハンドル、
	//				    移動先のファイル位置、
	//				    移動後のファイル位置を受け取らないためnullptr、
	//					ファイル先頭から移動する指定);

	if (!SetFilePointerEx(m_fileHandle, 
						  l_fileSize,
						  nullptr,
						  FILE_BEGIN))
	{
		assert				   (false && "BinaryFileの書き込み用ファイルサイズ設定位置の移動に失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	// 現在のファイル位置を終端として、MemoryMappedFileで書き込むためのファイルサイズを確保する
	// SetEndOfFile(ファイルハンドル);
	if (!SetEndOfFile(m_fileHandle))
	{
		assert				   (false && "BinaryFileの書き込み用ファイルサイズ確保に失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	m_mappedDataSize = a_fileSize;
	m_isWritable	 = k_isWriteableMappedFile;

	// CreateFileMappingW(マッピング対象のファイルハンドル、
	//					  セキュリティ属性、
	//					  読み書き可能ページとして作成する指定、
	//					  最大サイズ上位32bit、0ならファイルサイズを使用、
	//					  最大サイズ下位32bit、0ならファイルサイズを使用、
	//					  名前付きマッピング);

	m_fileMappingHandle = CreateFileMappingW(m_fileHandle,
											 nullptr,
											 PAGE_READWRITE,
											 k_mappingMAXSizeHighUseFileSize,
											 k_mappingMAXSizeLowUseFileSize,
											 nullptr);

	if (!m_fileMappingHandle)
	{
		assert				   (false && "BinaryFileの書き込み用ファイルマッピング作成に失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	// MapViewOfFile(マッピングオブジェクトのハンドル、
	//				 読み書き可能でビューを作成する指定、
	//				 ファイルオフセット上位32bit、先頭から書き込むため0、
	//				 ファイルオフセット下位32bit、先頭から書き込むため0、
	//				 マップするサイズ、0ならファイル全体);

	m_mappedData = static_cast<std::uint8_t*>(MapViewOfFile(m_fileMappingHandle,
															FILE_MAP_READ | FILE_MAP_WRITE,
															k_viewFileOffsetHighFromBegin,
															k_viewFileOffsetLowFromBegin,
															k_mapEntireFileSize));

	if (!m_mappedData)
	{
		assert				   (false && "BinaryFileの書き込み用メモリマップに失敗しました。");
		DestroyMemoryMappedFile();
		return false;
	}

	return true;
}

void FWK::Converter::BinaryFileConverterBase::DestroyMemoryMappedFile()
{
	if (m_mappedData)
	{
		if (m_isWritable)
		{
			// 書き込んだメモリ内容をファイルへ反映する
			// FlushViewOfFile(書き込んだメモリマップ先頭アドレス、
			//				   フラッシュするバイト数(0ならビュー全体));

			FlushViewOfFile(m_mappedData, k_flushEntireViewSize);
		}

		// メモリマップを解除する
		UnmapViewOfFile(m_mappedData);
		m_mappedData = nullptr;
	}

	// 使用中のハンドルを破棄
	if (m_fileMappingHandle)
	{
		// ファイルやマッピングのハンドルを閉じる
		// CloseHandle(ハンドル);

		CloseHandle(m_fileMappingHandle);
		m_fileMappingHandle = nullptr;
	}

	if (m_fileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_fileHandle);
		m_fileHandle = INVALID_HANDLE_VALUE;
	}

	m_mappedDataSize = k_initialMappedDataSize;
	m_isWritable	 = k_isReadOnlyMappedFile;
}