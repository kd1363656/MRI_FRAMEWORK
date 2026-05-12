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

	// ファイルを読み込めるかを確認
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

	auto l_fileSizeHigh = k_fileSizeHigh;

	// GetFileSize()がINVALID_FILE_SIZEを返したときに、
	// 本当にエラーか判定するため、事前にエラー状態をリセットする
	SetLastError(NO_ERROR);

	// GetFileSize(ファイルハンドル、
	//			   ファイルサイズ上位32bitの受取先);

	const auto l_fileSizeLow = GetFileSize(m_fileHandle, &l_fileSizeHigh);

	if (l_fileSizeLow == INVALID_FILE_SIZE)
	{
		const auto l_error = GetLastError();

		if (l_error != NO_ERROR)
		{
			assert				   (false && "BinaryFileのファイルサイズ取得に失敗しました。");
			DestroyMemoryMappedFile();

			return false;
		}
	}

	// ファイルサイズはLow 32bit / High 32bitに分かれて返るため、
	// High側を上位32bitへ移動して64bitのサイズに戻す
	m_mappedDataSize = (static_cast<std::uint64_t>(l_fileSizeHigh) << k_highDWORDShiftBitCount) | static_cast<std::uint64_t>(l_fileSizeLow);

	m_isWritable = k_isReadOnlyMappedFile;

	if (m_mappedDataSize == k_emptyMappedDataSize)
	{
		assert				   (false && "BinaryFileの読み込み用ファイルサイズが0です。");
		DestroyMemoryMappedFile();

		return false;
	}

	// マッピングオブジェクトを作成
	// CreateFileMappingW(マッピング対象のファイルハンドル、
	//					  セキュリティ属性、
	//					  読み込み専用ページとして作成する指定、
	//					  最大サイズ上位32bit(0ならファイルサイズを使用)、
	//					  最大サイズ下位32bit(0ならファイルサイズを使用)、
	//					  名前付きマッピング);

	m_fileMappingHandle = CreateFileMappingW(m_fileHandle,
										     nullptr,
										     PAGE_READONLY,
											 k_mappingMaxSizeHighUseFileSize,
											 k_mappingMaxSizeLowUseFileSize,
											 nullptr);

	if (!m_fileMappingHandle)
	{
		assert				   (false && "BinaryFileの読み込み用ファイルマッピング作成に失敗しました。");
		DestroyMemoryMappedFile();

		return false;
	}

	// MapViewOfFile(マッピングオブジェクトのハンドル、
	//				 読み込み専用でビューを作成する指定、
	//				 ファイルオフセット上位32bit(0の場合ファイル先頭から読む)、
	//				 ファイルオフセット下位32bit(0の場合ファイル先頭から読む)、
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
	// 既に別のファイルを開いていた場合に備えて前のマッピングを破棄する
	DestroyMemoryMappedFile();

	if (a_filePath.empty())
	{
		assert(false && "BinaryFileの書き込み用ファイルパスが空です。");
		return false;
	}

	if (a_fileSize == k_emptyWriteFileSize)
	{
		assert(false && "BinaryFileの書き込みサイズが0です。");
		return false;
	}

	// 親ファイルパスが存在しなければフォルダごと作成
	if (const auto& l_parentPath = a_filePath.parent_path();
		!l_parentPath.empty())
	{
		std::filesystem::create_directories(l_parentPath);
	}

	// ファイルを作成する
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

	const auto l_fileSizeLow  = static_cast<DWORD>(a_fileSize);
		  auto l_fileSizeHigh = static_cast<LONG>(a_fileSize >> k_highDWORDShiftBitCount);

	SetLastError(NO_ERROR);

	// SetFilePointer(ファイルハンドル、
	//				  移動距離下位32bit、
	//				  移動距離上位32bit、
	//				  ファイル先頭から移動する指定);

	if (const auto l_setFilePointerResult = SetFilePointer(m_fileHandle,
													   l_fileSizeLow,
													   &l_fileSizeHigh,
													   FILE_BEGIN);
		l_setFilePointerResult == INVALID_SET_FILE_POINTER)
	{
		const auto l_error = GetLastError();

		if (l_error != NO_ERROR)
		{
			assert				   (false && "BinaryFileの書き込み用ファイルサイズ指定位置の移動に失敗しました。");
			DestroyMemoryMappedFile();

			return false;
		}
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
	m_isWritable     = k_isWriteMappedFile;

	// マッピングオブジェクト作成
	// CreateFileMappingW(マッピング対象のファイルハンドル、
	//					  セキュリティ属性、
	//					  読み書き可能ページとして作成する指定、
	//					  最大サイズ上位32bit、0ならファイルサイズを使用、
	//					  最大サイズ下位32bit、0ならファイルサイズを使用、
	//					  名前付きマッピング);

	m_fileMappingHandle = CreateFileMappingW(m_fileHandle,
											 nullptr,
											 PAGE_READWRITE,
											 k_mappingMaxSizeHighUseFileSize,
										     k_mappingMaxSizeLowUseFileSize,
											 nullptr);

	if (!m_fileMappingHandle)
	{
		assert				   (false && "BinaryFileの書き込み用ファイルマッピング作成に失敗しました。");
		DestroyMemoryMappedFile();

		return false;
	}

	// MapViewOfFile(マッピングオブジェクトのハンドル、
	//				 読み書き可能でビューを作成する指定、
	//				 ファイルオフセット上位32bit(0の場合ファイル先頭から書き込む指定)、
	//				 ファイルオフセット下位32bit(0の場合ファイル先頭から書き込む指定)、
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
			// FlushViewOfFile(書き込んだメモリマップ先頭アドレス、
			//				   フラッシュするバイト数、0ならビュー全体);

			FlushViewOfFile(m_mappedData, k_flushEntireViewSize);
		}

		// UnmapViewOfFile(マップ済みビューの先頭アドレス);
		UnmapViewOfFile(m_mappedData);

		m_mappedData = nullptr;
	}

	if (m_fileMappingHandle)
	{
		// CloseHandle(閉じるWinAPIハンドル);
		CloseHandle(m_fileMappingHandle);

		m_fileMappingHandle = nullptr;
	}

	if (m_fileHandle != INVALID_HANDLE_VALUE)
	{
		// CloseHandle(閉じるWinAPIハンドル);
		CloseHandle(m_fileHandle);

		m_fileHandle = INVALID_HANDLE_VALUE;
	}

	m_mappedDataSize = k_emptyMappedDataSize;
	m_isWritable     = k_isInitialWritable;
}
