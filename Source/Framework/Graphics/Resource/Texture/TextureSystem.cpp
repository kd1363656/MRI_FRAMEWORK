#include "TextureSystem.h"

void FWK::Graphics::TextureSystem::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_textureSystemJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::TextureSystem::Create(const Device&			                   a_device, 
										  const GPUMemoryAllocator&                a_gpuMemoryAllocator,
												DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
												UploadSystem&					   a_uploadSystem)
{
	if (!m_textureStorage.Create())
	{
		assert(false && "AssetStorageの作成に失敗したため、TextureSystemの作成処理に失敗しました。");
		return false;
	}

	std::vector<Struct::TextureBatchUploadRecord> l_defaultTextureBatchUploadRecordList = {};

	l_defaultTextureBatchUploadRecordList.reserve(Constant::k_defaultTextureCount);

	for (std::uint32_t l_defaultTextureTypeIndex = 0U; l_defaultTextureTypeIndex < static_cast<std::uint32_t>(Enum::DefaultTextureType::Count); ++l_defaultTextureTypeIndex)
	{
		const auto l_defaultTextureType = static_cast<Enum::DefaultTextureType>(l_defaultTextureTypeIndex);

		Struct::TextureBatchUploadRecord l_defaultTextureBatchUploadRecord = {};

		if (!m_defaultTextureBuilder.CreateDefaultTextureBatchUploadRecord(a_device,
																		   a_gpuMemoryAllocator,
																	       m_textureBatchUploadRecordBuilder,
																		   l_defaultTextureType,
																		   a_srvDescriptorPool,
																		   l_defaultTextureBatchUploadRecord))
		{
			assert(false && "デフォルトテクスチャの作成に失敗しました。");
			return false;
		}

		const auto l_defaultTextureRecordIndex = static_cast<std::size_t>(l_defaultTextureType);

		m_defaultTextureRecordList[l_defaultTextureRecordIndex] = l_defaultTextureBatchUploadRecord.m_textureRecord;

		l_defaultTextureBatchUploadRecordList.emplace_back(std::move(l_defaultTextureBatchUploadRecord));
	}

	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(l_defaultTextureBatchUploadRecordList))
	{
		assert(false && "デフォルトテクスチャのGPU転送に失敗しました。");
		return false;
	}

	return true;
}

FWK::Struct::TextureLoadResult FWK::Graphics::TextureSystem::LoadTextureForBatchUpload(const Device&			                a_device,
																				       const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																				       const std::filesystem::path&			    a_filePath,
																						     DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	Struct::TextureLoadResult l_textureLoadResult = {};

	// 読み込めるファイルかどうかを確認
	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerDDSExtension)) { return l_textureLoadResult; }

	const auto& l_filePath = a_filePath.wstring();
	
	// 既に登録済みのテクスチャなら再度ロード申請する必要がないのでreturn
	if (const auto& l_record = m_textureStorage.FindVALRecord(l_filePath).lock())
	{
		if (!AddTextureReference(l_record))
		{
			assert(false && "登録済みテクスチャの参照数加算に失敗したため、テクスチャ読み込み処理に失敗しました。");
			return l_textureLoadResult;
		}
		
		l_textureLoadResult.m_storageID     = l_record->GetVALStorageID();
		l_textureLoadResult.m_textureRecord = l_record;

		return l_textureLoadResult;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要がないためreturn
	if (const auto& l_itr = m_pendingTextureBatchUploadRecordMap.find(l_filePath);
		l_itr != m_pendingTextureBatchUploadRecordMap.end())
	{
		const auto& l_textureRecord = l_itr->second.m_textureRecord;

		if (!l_textureRecord)
		{
			assert(false && "該当するStorageIDのテクスチャーレコードが無効のため、StorageIDを返せませんでした。");
			return l_textureLoadResult;
		}

		// すでに登録予約済みのテクスチャが再度登録されたら参照カウントを増やす
		l_textureRecord->AddReferenceCount();

		l_textureLoadResult.m_storageID     = l_textureRecord->GetVALStorageID();
		l_textureLoadResult.m_textureRecord = l_textureRecord;

		return l_textureLoadResult;
	}	
	
	DirectX::ScratchImage l_scratchImage = {};
	DirectX::TexMetadata  l_texMetadata  = {};

	// まずはテクスチャをロードしてロードできるかどうかを確認
	if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath))
	{
		assert(false && "DDSテクスチャ読み込みに失敗したため、テクスチャのバッチロード処理に失敗しました。");
		return l_textureLoadResult;
	}

	Struct::TextureBatchUploadRecord l_textureBatchUploadRecord = {};

	const auto l_allocateStorageID = m_textureStorage.AllocateStorageID();

	if (l_allocateStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "StorageIDの割り当てに失敗したため、テクスチャ読み込み処理に失敗しました。");
		return l_textureLoadResult;
	}

	// テクスチャを作成、管理するのに必要な情報全てを作成
	if (!m_textureBatchUploadRecordBuilder.CreateTextureBatchUploadRecord(l_scratchImage, 
																		  l_texMetadata,
																		  a_device,
																		  a_gpuMemoryAllocator,
																		  l_filePath,
																		  l_allocateStorageID,
																		  a_srvDescriptorPool,
																		  l_textureBatchUploadRecord))
	{
		// テクスチャアップロード情報作成に失敗したなら
		// StorageIDを開放しておく
		m_textureStorage.ReleaseStorageID(l_allocateStorageID);

		assert(false && "テクスチャアップロード情報の作成に失敗したため、バッチテクスチャ登録に失敗しました。");
		return l_textureLoadResult;
	}

	const auto& l_textureRecord = l_textureBatchUploadRecord.m_textureRecord;

	if (!l_textureRecord) 
	{
		// Allocate済みのStorageIDなので、失敗時は返却しておく
		m_textureStorage.ReleaseStorageID(l_allocateStorageID);

		assert(false && "TextureRecordが無効のため、バッチテクスチャ登録に失敗しました。");
		return l_textureLoadResult; 
	}

	l_textureLoadResult.m_storageID     = l_textureRecord->GetVALStorageID();
	l_textureLoadResult.m_textureRecord = l_textureRecord;

	// 作成し終えたTextureBatchUploadRecordをリストに格納する
	m_pendingTextureBatchUploadRecordMap.try_emplace(l_filePath, std::move(l_textureBatchUploadRecord));

	return l_textureLoadResult;
}

void FWK::Graphics::TextureSystem::LoadPendingTexturesAndWait(UploadSystem& a_uploadSystem)
{
	// ロード待ちテクスチャが一つもなければreturn;
	if (m_pendingTextureBatchUploadRecordMap.empty()) { return; }

	// ロード申請が来ていたテクスチャを一括ロードする
	if (!TextureCopyBatch(a_uploadSystem))
	{
		assert(false && "ロード待ちテクスチャのバッチ登録に失敗しました。");
		return;
	}
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::TextureSystem::AddTextureReference(const std::weak_ptr<Graphics::TextureRecord>& a_textureRecord)
{
	if (!m_textureStorage.AddReference(a_textureRecord))
	{
		assert(false && "AssetStorageでの参照数加算に失敗したため、テクスチャ参照数加算に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Graphics::TextureSystem::ReleaseTextureReference(const std::weak_ptr<Graphics::TextureRecord>& a_textureRecord, const DirectCommandQueue& a_directCommandQueue, DeferredResourceReleaseQueue& a_deferredReleaseQueue)
{
	if (!m_textureStorage.ReleaseReference(a_textureRecord, a_directCommandQueue, a_deferredReleaseQueue))
	{
		assert(false && "AssetStorageでの参照数減算に失敗したため、テクスチャ解放予約に失敗しました。");
		return false;
	}

	return true;
}

std::weak_ptr<FWK::Graphics::TextureRecord> FWK::Graphics::TextureSystem::FindVALDefaultTextureRecord(const Enum::DefaultTextureType a_defaultTextureType) const
{
	const auto l_defaultTextureRecordIndex = static_cast<std::size_t>(a_defaultTextureType);

	if (l_defaultTextureRecordIndex >= m_defaultTextureRecordList.size())
	{
		assert(false && "DefaultTextureTypeが範囲外です。");
		return {};
	}

	return m_defaultTextureRecordList[l_defaultTextureRecordIndex];
}
std::weak_ptr<FWK::Graphics::TextureRecord> FWK::Graphics::TextureSystem::FindVALTextureRecord(const std::wstring& a_filePath) const
{
	return m_textureStorage.FindVALRecord(a_filePath);
}

bool FWK::Graphics::TextureSystem::TextureCopyBatch(UploadSystem& a_uploadSystem)
{
	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(m_pendingTextureBatchUploadRecordMap))
	{
		assert(false && "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。");
		return false;
	}

	for (const auto& [l_filePath, l_pendingTextureBatchUploadRecord] : m_pendingTextureBatchUploadRecordMap)
	{
		auto& l_textureRecord = l_pendingTextureBatchUploadRecord.m_textureRecord;

		if (!l_textureRecord)
		{	
			assert(false && "TextureRecordが無効のため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		if (!m_textureStorage.RegisterRecord(l_textureRecord, l_filePath))
		{
			assert(false && "TextureRecordの登録に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}
	}

	// そのフレーム内でロードすべきテクスチャをすべてロードし終えた状態なのでクリア
	m_pendingTextureBatchUploadRecordMap.clear();

	return true;
}