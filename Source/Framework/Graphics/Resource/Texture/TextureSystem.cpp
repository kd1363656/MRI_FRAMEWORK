#include "TextureSystem.h"

void FWK::Graphics::TextureSystem::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_textureSystemJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::TextureSystem::Create()
{
	if (!m_textureStorage.Create(m_storageIDAllocatorCapacity))
	{
		assert(false && "AssetStorageの作成に失敗したため、TextureSystemの作成処理に失敗しました。");
		return false;
	}

	return true;
}

FWK::TypeAlias::StorageID FWK::Graphics::TextureSystem::LoadTextureForBatchUpload(const Device&			                   a_device,
																				  const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																				  const std::filesystem::path&			   a_filePath,
																						DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	// 読み込めるファイルかどうかを確認
	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerDDSExtension))
	{
		assert(false && "テクスチャファイルパスが読み込めるファイルパスではありません。");
		return Constant::k_invalidStorageID;
	}

	const auto& l_filePath  = a_filePath.wstring						   ();
	const auto  l_storageID = m_textureStorage.FindVALStorageIDFromFilePath(l_filePath);

	// 既に登録済みのテクスチャなら再度ロード申請する必要がないのでreturn
	if (l_storageID == Constant::k_invalidStorageID)
	{
		if (!AddTextureReference(l_storageID))
		{
			assert(false && "登録済みテクスチャの参照数加算に失敗したため、テクスチャ読み込み処理に失敗しました。");
			return Constant::k_invalidStorageID;
		}

		return l_storageID;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要がないためreturn
	if (const auto& l_itr = m_pendingTextureBatchUploadRecordMap.find(l_filePath);
		l_itr != m_pendingTextureBatchUploadRecordMap.end())
	{
		// すでに登録予約済みのテクスチャが再度登録されたら参照カウントを増やす
		++l_itr->second.m_textureRecord.m_referenceCount;

		return l_itr->second.m_textureRecord.m_storageID;
	}	
	
	DirectX::ScratchImage l_scratchImage = {};
	DirectX::TexMetadata  l_texMetadata  = {};

	// まずはテクスチャをロードしてロードできるかどうかを確認
	if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath))
	{
		assert(false && "DDSテクスチャ読み込みに失敗したため、テクスチャのバッチロード処理に失敗しました。");
		return Constant::k_invalidStorageID;
	}

	Struct::TextureBatchUploadRecord l_textureBatchUploadRecord = {};

	const auto l_storageID = m_textureStorage.Allocate();

	if (l_storageID == Constant::k_invalidStorageID)
	{
		assert(false && "StorageIDの割り当てに失敗したため、テクスチャ読み込み処理に失敗しました。");
		return Constant::k_invalidStorageID;
	}

	// テクスチャを作成、管理するのに必要な情報全てを作成
	if (!m_textureBatchUploadRecordBuilder.CreateTextureBatchUploadRecord(l_scratchImage, 
																		  l_texMetadata,
																		  a_device,
																		  a_gpuMemoryAllocator,
																		  l_filePath,
																		  l_storageID,
																		  a_srvDescriptorPool,
																		  l_textureBatchUploadRecord))
	{
		assert(false && "テクスチャアップロード情報の作成に失敗したため、バッチテクスチャ登録に失敗しました。");
		return Constant::k_invalidStorageID;
	}

	const auto l_storageID = l_textureBatchUploadRecord.m_textureRecord.m_storageID;

	// 作成し終えたTextureBatchUploadRecordをリストに格納する
	m_pendingTextureBatchUploadRecordMap.try_emplace(l_filePath, std::move(l_textureBatchUploadRecord));

	return l_storageID;
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

	// そのフレーム内でロードすべきテクスチャをすべてロードし終えた状態なのでクリア
	m_pendingTextureBatchUploadRecordMap.clear();
}

void FWK::Graphics::TextureSystem::ReleaseCompletedUnusedTexture(const DirectCommandQueue& a_directCommandQueue, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	const auto& l_completedFenceValue = a_directCommandQueue.FetchVALCompletedFenceValue();

	auto l_itr = m_textureRecordMap.begin();

	while (l_itr != m_textureRecordMap.end())
	{
		auto& l_textureRecord = l_itr->second;

		// まだ参照されているテクスチャは解放しない
		if (l_textureRecord.m_referenceCount > Constant::k_emptyAssetReferenceCount)
		{
			++l_itr;
			continue;
		}

		// 解放予約用のFence値が初期値なら解放しない
		if (l_textureRecord.m_retiredFenceValue == Constant::k_unusedFenceValue)
		{
			++l_itr;
			continue;
		}

		// GPUがまだこのテクスチャを利用している可能性があるため解放しない
		if (l_completedFenceValue < l_textureRecord.m_retiredFenceValue)
		{
			++l_itr;
			continue;
		}

		// TextureResourceを解放
		if (l_textureRecord.m_textureResource)
		{
			l_textureRecord.m_textureResource.Reset();
		}

		// SRV用ストレージIDを返却する
		if (l_textureRecord.m_srvStorageID != Constant::k_invalidStorageID)
		{
			a_srvDescriptorPool.Release(l_textureRecord.m_srvStorageID);
		}

		// ファイルパスから対応するStorageIDを見つるMapの要素を削除
		m_texturePathStorageIDMap.erase(l_textureRecord.m_filePath);

		// StorageIDを返却する
		m_storageIDAllocator.Release(l_textureRecord.m_storageID);

		// TextureRecordMapから削除する
		// erase()は削除した次のイテレーターを返す
		l_itr = m_textureRecordMap.erase(l_itr);
	}
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::TextureSystem::AddTextureReference(const TypeAlias::StorageID a_storageID)
{
	if (!m_textureStorage.AddReference(a_storageID))
	{
		assert(false && "AssetStorageでの参照数加算に失敗したため、テクスチャ参照数加算に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::TextureSystem::ReleaseTextureReference(const DirectCommandQueue& a_directCommandQueue, const TypeAlias::StorageID a_storageID)
{
	if (!m_textureStorage.ReleaseReference(a_directCommandQueue, a_storageID))
	{
		assert(false && "AssetStorageでの参照数減算に失敗したため、テクスチャ解放予約に失敗しました。");
		return false;
	}

	return true;
}

const FWK::Struct::TextureRecord* FWK::Graphics::TextureSystem::FindPTRTextureRecord(const TypeAlias::StorageID a_storageID) const
{
	return m_textureStorage.FindPTRRecord(a_storageID);
}

FWK::Struct::TextureRecord* FWK::Graphics::TextureSystem::FindMutablePTRTextureRecord(const TypeAlias::StorageID a_storageID)
{
	return m_textureStorage.FindMutablePTRRecord(a_storageID);
}

bool FWK::Graphics::TextureSystem::TextureCopyBatch(UploadSystem& a_uploadSystem)
{
	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(m_pendingTextureBatchUploadRecordMap))
	{
		assert(false && "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。");
		return false;
	}

	for (auto& [l_filePath, l_pendingTextureBatchUploadRecord] : m_pendingTextureBatchUploadRecordMap)
	{
		if (!m_textureStorage.RegisterRecord(l_filePath, l_pendingTextureBatchUploadRecord.m_textureRecord))
		{
			assert(false && "TextureRecordの登録に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}
	}

	return true;
}