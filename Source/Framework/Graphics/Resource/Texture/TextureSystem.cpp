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
	FWK_ASSERT_RETURN_VALUE_IF(!m_textureStorage.Create(), "AssetStorageの作成に失敗したため、TextureSystemの作成処理に失敗しました。", false)

	std::vector<Struct::TextureBatchUploadRecord> l_defaultTextureBatchUploadRecordList = {};

	l_defaultTextureBatchUploadRecordList.reserve(k_defaultTextureCount);

	for (std::uint32_t l_defaultTextureTypeIndex = 0U; l_defaultTextureTypeIndex < static_cast<std::uint32_t>(Enum::DefaultTextureType::Count); ++l_defaultTextureTypeIndex)
	{
		const auto l_defaultTextureType = static_cast<Enum::DefaultTextureType>(l_defaultTextureTypeIndex);

		Struct::TextureBatchUploadRecord l_defaultTextureBatchUploadRecord = {};

		FWK_ASSERT_RETURN_VALUE_IF(!m_defaultTextureBuilder.CreateDefaultTextureBatchUploadRecord(a_device,
																								  a_gpuMemoryAllocator,
																								  m_textureBatchUploadRecordBuilder,
																								  l_defaultTextureType,
																								  a_srvDescriptorPool,
																								  l_defaultTextureBatchUploadRecord),
																								  "デフォルトテクスチャの作成に失敗しました。",
																								  false)


		const auto l_defaultTextureRecordIndex = static_cast<std::size_t>(l_defaultTextureType);

		m_defaultTextureRecordList[l_defaultTextureRecordIndex] = l_defaultTextureBatchUploadRecord.m_textureRecord;

		l_defaultTextureBatchUploadRecordList.emplace_back(std::move(l_defaultTextureBatchUploadRecord));
	}

	FWK_ASSERT_RETURN_VALUE_IF(!a_uploadSystem.SubmitTextureCopyBatchAndWait(l_defaultTextureBatchUploadRecordList), "AssetStorageの作成に失敗したため、TextureSystemの作成処理に失敗しました。", false)

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
		FWK_ASSERT_RETURN_VALUE_IF(!AddTextureReference(l_record), "登録済みテクスチャの参照数加算に失敗したため、テクスチャ読み込み処理に失敗しました。", l_textureLoadResult)
	
		l_textureLoadResult.m_storageID     = l_record->GetVALStorageID();
		l_textureLoadResult.m_textureRecord = l_record;

		return l_textureLoadResult;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要がないためreturn
	if (const auto& l_itr = m_pendingTextureBatchUploadRecordMap.find(l_filePath);
		l_itr != m_pendingTextureBatchUploadRecordMap.end())
	{
		const auto& l_textureRecord = l_itr->second.m_textureRecord;

		FWK_ASSERT_RETURN_VALUE_IF(!l_textureRecord, "該当するStorageIDのテクスチャーレコードが無効のため、StorageIDを返せませんでした。", l_textureLoadResult)

		// すでに登録予約済みのテクスチャが再度登録されたら参照カウントを増やす
		l_textureRecord->AddReferenceCount();

		l_textureLoadResult.m_storageID     = l_textureRecord->GetVALStorageID();
		l_textureLoadResult.m_textureRecord = l_textureRecord;

		return l_textureLoadResult;
	}	
	
	DirectX::ScratchImage l_scratchImage = {};
	DirectX::TexMetadata  l_texMetadata  = {};

	// まずはテクスチャをロードしてロードできるかどうかを確認
	FWK_ASSERT_RETURN_VALUE_IF(!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath), "DDSテクスチャ読み込みに失敗したため、テクスチャのバッチロード処理に失敗しました。", l_textureLoadResult)

	Struct::TextureBatchUploadRecord l_textureBatchUploadRecord = {};

	const auto l_allocateStorageID = m_textureStorage.AllocateStorageID();

	FWK_ASSERT_RETURN_VALUE_IF(l_allocateStorageID == Constant::k_invalidStorageID, "StorageIDの割り当てに失敗したため、テクスチャ読み込み処理に失敗しました。", l_textureLoadResult)

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

		FWK_ASSERT_RETURN_VALUE("テクスチャアップロード情報の作成に失敗したため、バッチテクスチャ登録に失敗しました。", l_textureLoadResult)
	}

	const auto& l_textureRecord = l_textureBatchUploadRecord.m_textureRecord;

	if (!l_textureRecord) 
	{
		// Allocate済みのStorageIDなので、失敗時は返却しておく
		m_textureStorage.ReleaseStorageID(l_allocateStorageID);

		FWK_ASSERT_RETURN_VALUE("TextureRecordが無効のため、バッチテクスチャ登録に失敗しました。", l_textureLoadResult)
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
	FWK_ASSERT_RETURN_IF(!TextureCopyBatch(a_uploadSystem), "ロード待ちテクスチャのバッチ登録に失敗しました。")
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::TextureSystem::AddTextureReference(const std::weak_ptr<Graphics::TextureRecord>& a_textureRecord)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_textureStorage.AddReference(a_textureRecord), "AssetStorageでの参照数加算に失敗したため、テクスチャ参照数加算に失敗しました。", false)

	return true;
}
bool FWK::Graphics::TextureSystem::ReleaseTextureReference(const std::weak_ptr<Graphics::TextureRecord>& a_textureRecord, const DirectCommandQueue& a_directCommandQueue, DeferredResourceReleaseQueue& a_deferredReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_textureStorage.ReleaseReference(a_textureRecord, a_directCommandQueue, a_deferredReleaseQueue), "AssetStorageでの参照数減算に失敗したため、テクスチャ解放予約に失敗しました。", false)

	return true;
}

std::weak_ptr<FWK::Graphics::TextureRecord> FWK::Graphics::TextureSystem::FindVALDefaultTextureRecord(const Enum::DefaultTextureType a_defaultTextureType) const
{
	const auto l_defaultTextureRecordIndex = static_cast<std::size_t>(a_defaultTextureType);

	FWK_ASSERT_RETURN_VALUE_IF(l_defaultTextureRecordIndex >= m_defaultTextureRecordList.size(), "DefaultTextureTypeが範囲外です。", {})

	return m_defaultTextureRecordList[l_defaultTextureRecordIndex];
}
std::weak_ptr<FWK::Graphics::TextureRecord> FWK::Graphics::TextureSystem::FindVALTextureRecord(const std::wstring& a_filePath) const
{
	return m_textureStorage.FindVALRecord(a_filePath);
}

bool FWK::Graphics::TextureSystem::TextureCopyBatch(UploadSystem& a_uploadSystem)
{
	FWK_ASSERT_RETURN_VALUE_IF(!a_uploadSystem.SubmitTextureCopyBatchAndWait(m_pendingTextureBatchUploadRecordMap), "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。", false)

	for (const auto& [l_filePath, l_pendingTextureBatchUploadRecord] : m_pendingTextureBatchUploadRecordMap)
	{
		auto& l_textureRecord = l_pendingTextureBatchUploadRecord.m_textureRecord;

		FWK_ASSERT_RETURN_VALUE_IF(!l_textureRecord,											  "TextureRecordが無効のため、バッチテクスチャ登録に失敗しました。",			 false)
		FWK_ASSERT_RETURN_VALUE_IF(!m_textureStorage.RegisterRecord(l_textureRecord, l_filePath), "TextureRecordの登録に失敗したため、バッチテクスチャ登録に失敗しました。", false)
	}

	// そのフレーム内でロードすべきテクスチャをすべてロードし終えた状態なのでクリア
	m_pendingTextureBatchUploadRecordMap.clear();

	return true;
}