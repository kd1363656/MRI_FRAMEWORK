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

	std::vector<UINT>		                 l_allocatedSRVIndexList   = {};
	std::vector<std::wstring>                l_filePathList		       = {};
	std::vector<TypeAlias::TextureID>        l_textureIDList           = {};
	std::vector<Struct::TextureRecord>       l_textureRecordList       = {};
	std::vector<Struct::TextureUploadRecord> l_textureUploadRecordList = {};

	l_allocatedSRVIndexList.reserve  (a_filePathSet.size());
	l_filePathList.reserve		     (a_filePathSet.size());
	l_textureIDList.reserve		     (a_filePathSet.size());
	l_textureRecordList.reserve	     (a_filePathSet.size());
	l_textureUploadRecordList.reserve(a_filePathSet.size());

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

		const auto l_srvIndex = a_srvDescriptorPool.Allocate();

		// 取得したアロケータの値が無効ならreturn;
		if (l_srvIndex == Constant::k_invalidDescriptorHeapIndex)
		{
			for (const auto l_allocatedSRVIndex : l_allocatedSRVIndexList)
			{
				a_srvDescriptorPool.Release(l_allocatedSRVIndex);
			}

			assert(false && "SRVインデックスの確保に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		l_allocatedSRVIndexList.emplace_back(l_srvIndex);

		// 取得したTextureIDが無効ならreturn
		const auto l_textureID = GenerateTextureID();

		if (l_textureID == Constant::k_invalidTextureID)
		{
			for (const auto l_allocatedSRVIndex : l_allocatedSRVIndexList)
			{
				a_srvDescriptorPool.Release(l_allocatedSRVIndex);
			}

			assert(false && "TextureIDの採番に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		DirectX::ScratchImage l_scratchImage = {};
		DirectX::TexMetadata  l_texMetadata  = {};

		if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath))
		{
			for (const auto l_allocatedSRVIndex : l_allocatedSRVIndexList)
			{
				a_srvDescriptorPool.Release(l_allocatedSRVIndex);
			}

			assert(false && "DDSテクスチャ読み込みに失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		Struct::TextureRecord		l_textureRecord		  = {};
		Struct::TextureUploadRecord l_textureUploadRecord = {};

		l_textureRecord.m_textureID    = l_textureID;
		l_textureRecord.m_srvIndex     = l_srvIndex;
		l_textureRecord.m_filePath     = l_filePath;
		l_textureRecord.m_currentState = D3D12_RESOURCE_STATE_COPY_DEST;

		if (!m_textureUploadRecordBuilder.CreateTextureUploadRecord(l_scratchImage,
																	l_texMetadata,	
																	a_device,
																	a_gpuMemoryAllocator,
																	a_srvDescriptorPool,
																	l_textureRecord,
																	l_textureUploadRecord))
		{
			for (const auto l_allocatedSRVIndex : l_allocatedSRVIndexList)
			{
				a_srvDescriptorPool.Release(l_allocatedSRVIndex);
			}

			assert(false && "テクスチャアップロード情報作成に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		l_filePathList.emplace_back           (l_filePath);
		l_textureIDList.emplace_back          (l_textureID);
		l_textureRecordList.emplace_back      (std::move(l_textureRecord));
		l_textureUploadRecordList.emplace_back(std::move(l_textureUploadRecord));
	}
	
	if (l_textureUploadRecordList.empty()) { return true; }

	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(l_textureUploadRecordList))
	{
		for (const auto l_allocatedSRVIndex : l_allocatedSRVIndexList)
		{
			a_srvDescriptorPool.Release(l_allocatedSRVIndex);
		}

		assert(false && "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。");
		return false;
	}

	for (std::size_t l_index = 0ULL; l_index < l_textureRecordList.size(); ++l_index)
	{
		m_texturePathMap.try_emplace  (l_filePathList [l_index], l_textureIDList[l_index]);
		m_textureRecordMap.try_emplace(l_textureIDList[l_index], std::move(l_textureRecordList[l_index]));
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