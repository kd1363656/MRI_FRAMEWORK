#include "TextureSystem.h"

void FWK::Graphics::TextureSystem::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_textureSystemJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::TextureSystem::Create()
{
	if (!m_textureIDAllocator.Create(m_textureIDAllocatorCapacity))
	{
		assert(false && "テクスチャIDアロケータの作成に失敗したため、TextureSystemの作成処理に失敗しました。。");
		return false;
	}

	return true;
}

FWK::TypeAlias::TextureID FWK::Graphics::TextureSystem::LoadTextureForBatchUpload(const Device&			                   a_device,
																				  const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																				  const std::filesystem::path&			   a_filePath,
																						DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	// 読み込めるファイルかどうかを確認
	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerDDSExtension))
	{
		assert(false && "テクスチャファイルパスが読み込めるファイルパスではありません。");
		return Constant::k_invalidTextureID;
	}

	const auto& l_filePath = a_filePath.wstring();

	// 既に登録済みのテクスチャなら再度ロード申請する必要がないのでreturn
	if (const auto& l_itr = m_texturePathMap.find(l_filePath);
		l_itr != m_texturePathMap.end())
	{
		if (!AddTextureReference(l_itr->second))
		{
			assert(false && "登録済みテクスチャの参照数加算に失敗したため、テクスチャ読み込み処理に失敗しました。");
			return Constant::k_invalidTextureID;
		}

		return l_itr->second;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要がないためreturn
	if (const auto& l_itr = m_pendingTextureBatchUploadRecordMap.find(l_filePath);
		l_itr != m_pendingTextureBatchUploadRecordMap.end())
	{
		// すでに予約登録予約済みテクスチャが再度登録されたら参照カウントを増やす
		++l_itr->second.m_textureRecord.m_referenceCount;

		return l_itr->second.m_textureRecord.m_textureID;
	}	
	
	DirectX::ScratchImage l_scratchImage = {};
	DirectX::TexMetadata  l_texMetadata  = {};

	// まずはテクスチャをロードしてロードできるかどうかを確認
	if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath))
	{
		assert(false && "DDSテクスチャ読み込みに失敗したため、テクスチャのバッチロード処理に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	Struct::TextureBatchUploadRecord l_textureBatchUploadRecord = {};

	// テクスチャを作成、管理するのに必要な情報全てを作成
	if (!m_textureBatchUploadRecordBuilder.CreateTextureBatchUploadRecord(l_scratchImage, 
																		  l_texMetadata,
																		  a_device,
																		  a_gpuMemoryAllocator,
																		  l_filePath,
																		  a_srvDescriptorPool,
																		  m_textureIDAllocator,
																		  l_textureBatchUploadRecord))
	{
		assert(false && "テクスチャアップロード情報の作成に失敗したため、バッチテクスチャ登録に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	const auto l_textureID = l_textureBatchUploadRecord.m_textureRecord.m_textureID;

	// 作成し終えたTextureBatchUploadRecordをリストに格納する
	m_pendingTextureBatchUploadRecordMap.try_emplace(l_filePath, std::move(l_textureBatchUploadRecord));

	return l_textureID;
}

void FWK::Graphics::TextureSystem::LoadPendingTexturesAndWait(UploadSystem& a_uploadSystem)
{
	// std::unordered_set内にロードするテクスチャのファイルパスが一つもなければreturn
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
	const auto& l_completedFernceValue   = a_directCommandQueue.FetchVALCompletedFenceValue();

	auto l_itr = m_textureRecordMap.begin();

	while (l_itr != m_textureRecordMap.end())
	{
		auto& l_textureRecord = l_itr->second;

		// まだ参照されているテクスチャは解放しない
		if (l_textureRecord.m_referenceCount > Constant::k_emptyTextureReferenceCount)
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
		if (l_completedFernceValue < l_textureRecord.m_retiredFenceValue)
		{
			++l_itr;
			continue;
		}

		// TextureResourceを開放
		if (l_textureRecord.m_textureResource)
		{
			l_textureRecord.m_textureResource.Reset();
		}

		// SRV用ディスクリプタインデックスを返却する
		if (l_textureRecord.m_srvIndex != Constant::k_invalidDescriptorHeapIndex)
		{
			a_srvDescriptorPool.Release(l_textureRecord.m_srvIndex);
		}

		// ファイルパスからそれに対応するTextureIDを見つけ出すMapの要素を削除
		m_texturePathMap.erase(l_textureRecord.m_filePath);

		// TextureIDを返却する
		m_textureIDAllocator.Release(l_textureRecord.m_textureID);

		// TextureRecordMapから削除する
		// erase()は削除した次のイテレーターを返す
		l_itr = m_textureRecordMap.erase(l_itr);
	}
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::TextureSystem::AddTextureReference(const TypeAlias::TextureID a_textureID)
{
	if (a_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "無効なTextureIDが指定されたため、テクスチャ参照数加算に失敗しました。");
		return false;
	}

	auto* l_textureRecord = FindMutablePTRTextureRecord(a_textureID);

	if (!l_textureRecord)
	{
		assert(false && "指定されたTextureIDのTextureRecordが見つからないため、テクスチャ参照数加算に失敗しました。");
		return false;
	}

	// 参照数を加算
	++l_textureRecord->m_referenceCount;

	return true;
}
bool FWK::Graphics::TextureSystem::ReleaseTextureReference(const DirectCommandQueue& a_directCommandQueue, const TypeAlias::TextureID a_textureID)
{
	if (a_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "無効なTextureIDが指定されたため、テクスチャ解放予約に失敗しました。");
		return false;
	}

	auto* l_textureRecord = FindMutablePTRTextureRecord(a_textureID);

	if (!l_textureRecord)
	{
		assert(false && "指定されたTextureIDのTextureRecordが見つからないため、テクスチャ解放予約に失敗しました。");
		return false;
	}

	if (l_textureRecord->m_referenceCount == Constant::k_emptyTextureReferenceCount)
	{
		assert(false && "参照数が0のTextureRecordに対してさらに解放要求が行われました");
		return false;
	}

	--l_textureRecord->m_referenceCount;

	// まだ利用者が残っているなら何もしない
	if (l_textureRecord->m_referenceCount > Constant::k_emptyTextureReferenceCount) { return true; }

	const auto& l_lastSignaledFenceValue = a_directCommandQueue.FetchREFLastSignaledFenceValue();

	// GPUに対して発行されたフェンス値を格納する
	// GPUのフェンス値がこの格納されたフェンス値を超えていたら安全にリリースできるということ(GPu側での使用が終わっているから)
	l_textureRecord->m_retiredFenceValue = l_lastSignaledFenceValue;

	return true;
}

const FWK::Struct::TextureRecord* FWK::Graphics::TextureSystem::FindPTRTextureRecord(const TypeAlias::TextureID a_textureID) const
{
	if (a_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDが無効のため、TextureRecordの取得に失敗しました。");
		return nullptr;
	}

	const auto& l_itr = m_textureRecordMap.find(a_textureID);

	if (l_itr == m_textureRecordMap.end())
	{
		assert(false && "指定されたTextureIDに対応するTextureRecordが見つかりませんでした。");
		return nullptr;
	}

	return &l_itr->second;
}

FWK::Struct::TextureRecord* FWK::Graphics::TextureSystem::FindMutablePTRTextureRecord(const TypeAlias::TextureID a_textureID)
{
	if (a_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDが無効のため、TextureRecordの取得に失敗しました。");
		return nullptr;
	}

	const auto& l_itr = m_textureRecordMap.find(a_textureID);

	if (l_itr == m_textureRecordMap.end())
	{
		assert(false && "指定されたTextureIDに対応するTextureRecordが見つかりませんでした。");
		return nullptr;
	}

	return &l_itr->second;
}

bool FWK::Graphics::TextureSystem::TextureCopyBatch(UploadSystem& a_uploadSystem)
{
	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(m_pendingTextureBatchUploadRecordMap))
	{
		assert(false && "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。。");
		return false;
	}

	for (auto& [l_filePath, l_pendingTextureBatchUploadRecord] : m_pendingTextureBatchUploadRecordMap)
	{
		auto& l_textureRecord = l_pendingTextureBatchUploadRecord.m_textureRecord;

		m_texturePathMap.try_emplace  (l_filePath,					l_textureRecord.m_textureID);
		m_textureRecordMap.try_emplace(l_textureRecord.m_textureID,	std::move(l_pendingTextureBatchUploadRecord.m_textureRecord));
	}

	return true;
}