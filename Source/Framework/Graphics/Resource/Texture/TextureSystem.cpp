#include "TextureSystem.h"

bool FWK::Graphics::TextureSystem::RequestTextureLoad(const std::filesystem::path& a_filePath)
{
	if (a_filePath.empty())
	{
		assert(false && "読み込み申請されたテクスチャファイルパスが空のため、テクスチャ読み込み申請に失敗しました。");
		return false;
	}

	// 拡張子が.ddsでなければreturn
	if (a_filePath.extension() != Constant::k_lowerDDSExtension)
	{
		assert(false && "読み込み申請されたテクスチャファイルパスの拡張子が.ddsではなく、テクスチャ読み込み申請に失敗しました。");
		return false;
	}

	const auto& l_filePath = a_filePath.wstring();

	// 既に登録済みのテクスチャなら再度ロード申請する必要はない
	if (const auto& l_itr = m_texturePathMap.find(l_filePath);
		l_itr != m_texturePathMap.end())
	{
		return true;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要はない
	if (const auto& l_itr = m_pendingTextureFilePathSet.find(l_filePath);
		l_itr != m_pendingTextureFilePathSet.end())
	{
		return true;
	}

	m_pendingTextureFilePathSet.emplace(l_filePath);

	return true;
}

void FWK::Graphics::TextureSystem::EarlyUpdate(const Device&			                a_device, 
											   const GPUMemoryAllocator&                a_gpuMemoryAllocator,
													 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
													 UploadSystem&						a_uploadSystem)
{
	// テクスチャの読み込み申請が一つでもあれば実行
	ProcessPendingTextureLoadsAndWait(a_device,
									  a_gpuMemoryAllocator,	
									  a_srvDescriptorPool,
									  a_uploadSystem);
}

bool FWK::Graphics::TextureSystem::RegisterTextureBatch(const Device&			                 a_device,
														const GPUMemoryAllocator&                a_gpuMemoryAllocator,
														const PendingTextureFilePathSet&         a_filePathSet,
															  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
															  UploadSystem&						 a_uploadSystem)
{
	if (a_filePathSet.empty())
	{
		assert(false && "登録対象のテクスチャファイルパスSetが空のため、バッチテクスチャ登録に失敗しました。");
		return false;
	}

	std::vector<Struct::TextureBatchUploadRecord> l_textureBatchUploadRecordList = {};
	
	l_textureBatchUploadRecordList.reserve(a_filePathSet.size());

	for (const auto& l_filePath : a_filePathSet)
	{
		if (l_filePath.empty())
		{
			assert(false && "登録対象のテクスチャファイルパスが空のため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		// 既に登録済みのテクスチャなら再度登録する必要はない
		if (const auto& l_itr = m_texturePathMap.find(l_filePath);
			l_itr != m_texturePathMap.end())
		{
			continue;
		}

		Struct::TextureBatchUploadRecord l_textureBatchUploadRecord = {};

		auto& l_textureRecord = l_textureBatchUploadRecord.m_textureRecord;

		l_textureRecord.m_srvIndex = a_srvDescriptorPool.Allocate();

		// 取得したSRVインデックスが無効ならreturn;
		if (l_textureRecord.m_srvIndex == Constant::k_invalidDescriptorHeapIndex)
		{
			assert(false && "SRVインデックスの確保に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		l_textureRecord.m_textureID = GenerateTextureID();
		
		// 取得したTextureIDが無効ならreturn
		if (l_textureRecord.m_textureID == Constant::k_invalidTextureID)
		{
			assert(false && "TextureIDの採番に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		DirectX::ScratchImage l_scratchImage = {};
		DirectX::TexMetadata  l_texMetadata  = {};

		if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath))
		{
			assert(false && "DDSテクスチャ読み込みに失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		l_textureBatchUploadRecord.m_textureRecord.m_filePath     = l_filePath;
		l_textureBatchUploadRecord.m_textureRecord.m_currentState = D3D12_RESOURCE_STATE_COPY_DEST;

		if (!m_textureRegistrationBuilder.CreateTextureUploadRecord(l_scratchImage,
																	l_texMetadata,	
																	a_device,
																	a_gpuMemoryAllocator,
																	a_srvDescriptorPool,
																	l_textureRecord,
																	l_textureBatchUploadRecord.m_textureUploadRecord))
		{
			assert(false && "テクスチャアップロード情報作成に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		l_textureBatchUploadRecordList.emplace_back(std::move(l_textureBatchUploadRecord));
	}
	
	if (l_textureBatchUploadRecordList.empty()) { return true; }

	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(l_textureBatchUploadRecordList))
	{
		assert(false && "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。");
		return false;
	}

	for (auto& l_record : l_textureBatchUploadRecordList)
	{
		auto& l_textureRecord = l_record.m_textureRecord;

		m_texturePathMap.try_emplace  (l_textureRecord.m_filePath,  l_textureRecord.m_textureID);
		m_textureRecordMap.try_emplace(l_textureRecord.m_textureID, std::move(l_record.m_textureRecord));
	}

	return true;
}

FWK::TypeAlias::TextureID FWK::Graphics::TextureSystem::GenerateTextureID()
{
	if (m_nextTextureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDが上限に到達したため、TextureIDの採番に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	// テクスチャIDを新規用に更新
	const auto l_textureID = m_nextTextureID;

	++m_nextTextureID;

	return l_textureID;
}

bool FWK::Graphics::TextureSystem::ProcessPendingTextureLoadsAndWait(const Device&			                  a_device,
																	 const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																		   DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
																		   UploadSystem&					  a_uploadSystem)
{
	if (m_pendingTextureFilePathSet.empty()) { return true; }

	if (!RegisterTextureBatch(a_device,
							  a_gpuMemoryAllocator,
							  m_pendingTextureFilePathSet,
							  a_srvDescriptorPool,
							  a_uploadSystem))
	{
		assert(false && "処理待ちテクスチャのバッチ登録に失敗しました。");
		
		// 同じ失敗を毎フレーム繰り返さないため、今回は保留中テクスチャファイルパスをクリアする
		m_pendingTextureFilePathSet.clear();
		return false;
	}

	m_pendingTextureFilePathSet.clear();

	return true;
}