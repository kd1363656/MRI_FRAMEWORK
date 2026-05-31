#include "StaticModelSystem.h"

void FWK::Graphics::StaticModelSystem::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_staticModelSystemJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::StaticModelSystem::Create()
{
	if (!m_staticModelStorage.Create())
	{
		assert(false && "AssetStorageの作成に失敗したため、StaticModelSystemの作成処理に失敗しました。");
		return false;
	}

	return true;
}

FWK::Struct::StaticModelResult FWK::Graphics::StaticModelSystem::LoadStaticModelForBatchUpload(const Device&			                a_device, 
																							   const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																							   const std::filesystem::path&             a_filePath, 
																									 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	Struct::StaticModelResult l_staticModelLoadResult = {};

	FWK_ASSERT_RETURN_VALUE_IF(!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerFBXExtension), "StaticModelのFBXファイルが存在しません。", l_staticModelLoadResult)

	const auto& l_filePath = a_filePath.wstring();

	// 既に登録済みのStaticModelなら再度ロード申請する必要がないのでreturn
	if (const auto& l_record = m_staticModelStorage.FindVALRecord(l_filePath).lock())
	{
		const auto l_foundStorageID = l_record->GetVALStorageID();

		// 参照カウントの加算
		FWK_ASSERT_RETURN_VALUE_IF(!AddStaticModelReference(l_record), "登録済みStaticModelの参照数加算に失敗したため、StaticModel読み込み処理に失敗しました。", l_staticModelLoadResult)

		l_staticModelLoadResult.m_storageID			= l_foundStorageID;
		l_staticModelLoadResult.m_staticModelRecord = l_record;

		return l_staticModelLoadResult;
	}

	// 既にPending中のStaticModelなら再度ロード申請する必要がないのでreturn
	if (const auto& l_itr = m_pendingStaticModelBatchUploadRecordMap.find(l_filePath);
		l_itr != m_pendingStaticModelBatchUploadRecordMap.end())
	{
		const auto& l_staticModelRecord = l_itr->second.m_staticModelRecord;

		FWK_ASSERT_RETURN_VALUE_IF(!l_staticModelRecord, "Pending中のStaticModelRecordが無効のため、StaticModel読み込み処理に失敗しました。", l_staticModelLoadResult)

		l_staticModelRecord->AddReferenceCount();

		l_staticModelLoadResult.m_storageID			= l_staticModelRecord->GetVALStorageID();
		l_staticModelLoadResult.m_staticModelRecord = l_staticModelRecord;

		return l_staticModelLoadResult;
	}

	const auto l_allocateStorageID = m_staticModelStorage.AllocateStorageID();

	if (l_allocateStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "StorageIDの割り当てに失敗したため、StaticModel読み込み処理に失敗しました。");
		return l_staticModelLoadResult;
	}

	auto l_staticModelRecord = std::make_shared<Graphics::StaticModelRecord>();

	l_staticModelRecord->SetFilePath      (l_filePath);
	l_staticModelRecord->SetStorageID     (l_allocateStorageID);
	l_staticModelRecord->SetReferenceCount(Constant::k_defaultAssetReferenceCount);

	// まずはバイナリーファイルから読み込み、読み込めなかった場合はFBXから読み込む
	if (!LoadStaticModel(a_filePath, *l_staticModelRecord))
	{
		m_staticModelStorage.ReleaseStorageID(l_allocateStorageID);
		FWK_ASSERT_RETURN_VALUE			     ("StaticModelの読み込みに失敗しました。", l_staticModelLoadResult)
	}

	// StaticModelのMaterialが参照しているTextureをロード予約する
	for (auto& l_modelMesh : l_staticModelRecord->GetREFModelData().m_modelMeshList)
	{
		const auto& l_modelMaterialAssetData   = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;
			  auto& l_modelMaterialRuntimeData = l_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData;

		l_modelMaterialRuntimeData.m_baseColorTexture = CreateMaterialTexture(a_filePath, l_modelMaterialAssetData.m_baseColorTextureFilePath, Enum::DefaultTextureType::BaseColor);
		l_modelMaterialRuntimeData.m_normalTexture    = CreateMaterialTexture(a_filePath, l_modelMaterialAssetData.m_normalTextureFilePath,	   Enum::DefaultTextureType::Normal);
	}

	Struct::StaticModelBatchUploadRecord l_staticModelBatchUploadRecord = {};

	l_staticModelBatchUploadRecord.m_staticModelRecord = l_staticModelRecord;

	// StaticModel用BufferResource / UploadBuffer / BufferUploadCommand / StructuredBuffer用SRVを作成
	if (!m_staticModelBatchUploadRecordBuilder.CreateStaticModelBatchUploadRecord(a_device,
																				  a_gpuMemoryAllocator,
																				  l_staticModelBatchUploadRecord.m_bufferUploadCommandList,
																				  a_srvDescriptorPool,
																				  *l_staticModelRecord))
	{
		m_staticModelStorage.ReleaseStorageID(l_allocateStorageID);
		FWK_ASSERT_RETURN_VALUE				 ("StaticModel用BufferUploadCommandの作成に失敗しました。", l_staticModelLoadResult)
	}
	
	l_staticModelLoadResult.m_storageID			= l_staticModelRecord->GetVALStorageID();
	l_staticModelLoadResult.m_staticModelRecord = l_staticModelRecord;

	m_pendingStaticModelBatchUploadRecordMap.try_emplace(l_filePath, std::move(l_staticModelBatchUploadRecord));

	return l_staticModelLoadResult;
}

void FWK::Graphics::StaticModelSystem::LoadPendingStaticModelAndWait(UploadSystem& a_uploadSystem)
{
	// ロード待ちStaticModelが一つもなければreturn
	if (m_pendingStaticModelBatchUploadRecordMap.empty()) { return; }

	// ロード申請が来ていたStaticModelを一括ロードする
	FWK_ASSERT_RETURN_IF(!StaticModelCopyBatch(a_uploadSystem), "ロード待ちStaticModelのバッチ登録に失敗しました。")

	// そのフレーム内でロードすべきStaticModelをすべてロードし終えた状態なのでクリア
	m_pendingStaticModelBatchUploadRecordMap.clear();
}

nlohmann::json FWK::Graphics::StaticModelSystem::Serialize() const
{
	return  m_staticModelSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::StaticModelSystem::AddStaticModelReference(const std::weak_ptr<Graphics::StaticModelRecord>& a_staticModelRecord)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelStorage.AddReference(a_staticModelRecord), "AssetStorageでの参照数加算に失敗したため、StaticModel参照数加算に失敗しました。", false)

	return true;
}

bool FWK::Graphics::StaticModelSystem::ReleaseStaticModelReference(const std::weak_ptr<Graphics::StaticModelRecord>& a_staticModelRecord, const DirectCommandQueue& a_directCommandQueue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelStorage.ReleaseReference(a_staticModelRecord, a_directCommandQueue, a_deferredResourceReleaseQueue), "AssetStorageでの参照数減算に失敗したため、StaticModel解放予約に失敗しました。", false)

	return true;
}

std::weak_ptr<FWK::Graphics::StaticModelRecord> FWK::Graphics::StaticModelSystem::FindVALStaticModelRecord(const std::wstring& a_filePath) const
{
	return m_staticModelStorage.FindVALRecord(a_filePath);
}

bool FWK::Graphics::StaticModelSystem::CreateStaticModelAssetFromFBX(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath, Graphics::StaticModelRecord& a_staticModelRecord)
{
	// ufbxを使用してメッシュやマテリアルを読み込む
	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelFBXLoader.LoadStaticModelFile(a_fbxFilePath, a_staticModelRecord), "StaticModelFBXLoaderによるFBX読み込みに失敗しました。", false)


	// meshoptimizerを使用して頂点とインデックスをGPUで扱いやすい配置へ最適化
	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelMeshOptimizer.OptimizeStaticModelRecord(a_staticModelRecord), "StaticModelMeshOptimizerによるStaticModelRecordの最適化に失敗しました。", false)
	
	// MeshShaderで扱うため、最適化済みの頂点とインデックスからMeshletDataを作成
	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelMeshletBuilder.BuildStaticModelRecordMeshletData(a_staticModelRecord), "StaticModelのMeshletData作成に失敗しました。", false)
	
	// バイナリーファイルとして保存
	FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelBinaryConverter.SaveStaticModelAsset(a_staticModelRecord, a_assetFilePath), "StaticModelAssetの保存に失敗しました。", false)
	
	return true;
}

std::shared_ptr<FWK::Graphics::Texture> FWK::Graphics::StaticModelSystem::CreateMaterialTexture(const std::filesystem::path& a_modelFilePath, const std::wstring& a_textureFilePath, const Enum::DefaultTextureType a_defaultTextureType) const
{
	auto l_texture = std::make_shared<Texture>();

	// TextureFilePathが空の場合は、Material種別にあった代用テクスチャを設定する
	if (a_textureFilePath.empty())
	{
		l_texture->SetupDefaultTexture(a_defaultTextureType);

		return l_texture;
	}

	std::filesystem::path l_textureFilePath = a_textureFilePath;

	// FBXから取得したTextureFilePathが相対パスの場合
	// ModelFilePathの親フォルダからの相対パスとして解決する
	if (l_textureFilePath.is_relative())
	{
		l_textureFilePath = a_modelFilePath.parent_path() / l_textureFilePath;
	}

	l_texture->Load(l_textureFilePath);

	return l_texture;
}

bool FWK::Graphics::StaticModelSystem::LoadStaticModel(const std::filesystem::path& a_filePath, Graphics::StaticModelRecord& a_staticModelRecord)
{
	auto& l_modelData = a_staticModelRecord.GetREFModelData();

	FWK_ASSERT_RETURN_VALUE_IF(a_filePath.empty(),					 "StaticModelのFBXファイルパスが空です。",   false)
	FWK_ASSERT_RETURN_VALUE_IF(!std::filesystem::exists(a_filePath), "StaticModelのFBXファイルが存在しません。", false)

	// アセットデータファイルの存在確認
	const auto& l_assetFilePath = Utility::File::CreateFilePathByReplaceExtension(a_filePath, Constant::k_lowerAssetExtension);

	// .assetの更新日時がFBXよりも古ければ使えないバイナリーファイルと判定
	if (CanUseStaticModelAsset(a_filePath, l_assetFilePath))
	{
		// バイナリーファイルを読み込めるなら読み込む
		if (LoadStaticModelAsset(l_assetFilePath, a_staticModelRecord)) { return true; }

		// .assetが存在していても、読み込みに失敗した場合は壊れている可能性があるためFBXから再生成する
		l_modelData.m_modelMeshList.clear();
	}

	// バイナリーファイルが使用できなければufbxを使用してFBXモデルを読み込む
	return CreateStaticModelAssetFromFBX(a_filePath, l_assetFilePath, a_staticModelRecord);
}

bool FWK::Graphics::StaticModelSystem::LoadStaticModelAsset(const std::filesystem::path& a_assetFilePath, Graphics::StaticModelRecord& a_staticModelRecord)
{
	auto& l_modelData = a_staticModelRecord.GetREFModelData();

	l_modelData.m_modelMeshList.clear();

	if (!m_staticModelBinaryConverter.LoadStaticModelAsset(a_staticModelRecord, a_assetFilePath)) { return false; }

	return true;
}

bool FWK::Graphics::StaticModelSystem::CanUseStaticModelAsset(const std::filesystem::path& a_fbxFilePath, const std::filesystem::path& a_assetFilePath) const
{
	if (!std::filesystem::exists(a_assetFilePath)) { return false; }

	const auto& l_fbxLastWriteTime   = std::filesystem::last_write_time(a_fbxFilePath);
	const auto& l_assetLastWriteTime = std::filesystem::last_write_time(a_assetFilePath);

	// FBXのほうが新しい場合、assetは古いので使わない
	if (l_assetLastWriteTime < l_fbxLastWriteTime) { return false; }

	return true;
}

bool FWK::Graphics::StaticModelSystem::StaticModelCopyBatch(UploadSystem& a_uploadSystem)
{
	FWK_ASSERT_RETURN_VALUE_IF(!a_uploadSystem.SubmitStaticModelBufferCopyBatchAndWait(m_pendingStaticModelBatchUploadRecordMap), "UploadSystemでのStaticModel用BufferResourceのバッチコピーに失敗したため、StaticModelのバッチ登録に失敗しました。", false)

	for (const auto& [l_filePath, l_staticModelBatchUploadRecord] : m_pendingStaticModelBatchUploadRecordMap)
	{
		const auto& l_staticModelRecord = l_staticModelBatchUploadRecord.m_staticModelRecord;

		FWK_ASSERT_RETURN_VALUE_IF(!l_staticModelRecord,												  "StaticModelRecordが無効のため、StaticModelのバッチ登録に失敗しました。",		    false)
		FWK_ASSERT_RETURN_VALUE_IF(!m_staticModelStorage.RegisterRecord(l_staticModelRecord, l_filePath), "StaticModelRecordの登録に失敗したため、StaticModelのバッチ登録に失敗しました。", false)
	}

	return true;
}