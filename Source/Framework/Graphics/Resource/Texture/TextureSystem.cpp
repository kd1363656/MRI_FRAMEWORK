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
	if (a_filePath.empty())
	{
		assert(false && "読み込み申請されたテクスチャファイルパスが空のため、テクスチャ読み込み申請に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	// 拡張子が.ddsでなければreturn
	if (a_filePath.extension() != Constant::k_lowerDDSExtension)
	{
		assert(false && "読み込み申請されたテクスチャファイルパスの拡張子が.ddsではなく、テクスチャ読み込み申請に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	const auto& l_filePath = a_filePath.wstring();

	// 既に登録済みのテクスチャなら再度ロード申請する必要がないのでreturn
	if (const auto& l_itr = m_texturePathMap.find(l_filePath);
		l_itr != m_texturePathMap.end())
	{
		return l_itr->second;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要がないためreturn
	if (const auto& l_itr = m_pendingTextureBatchUploadRecordMap.find(l_filePath);
		l_itr != m_pendingTextureBatchUploadRecordMap.end())
	{
		return l_itr->second.m_textureRecord.m_textureID;;
	}	
	
	DirectX::ScratchImage l_scratchImage = {};
	DirectX::TexMetadata  l_texMetadata  = {};

	// まずはテクスチャをロードしてロードできるかどうかを確認
	if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_filePath))
	{
		assert(false && "DDSテクスチャ読み込みに失敗したため、テクスチャのバッチロード処理に失敗しました。");
		return false;
	}

	Struct::TextureBatchUploadRecord l_textureBatchUploadRecord = {};

	// テクスチャを作成、管理するのに必要な情報全てを作成
	if (!m_textureBatchUploadRecordBuilder.CreateTextureBatchUploadRecordBuilder(l_scratchImage, 
																				 l_texMetadata,
																				 a_device,
																				 a_gpuMemoryAllocator,
																				 a_srvDescriptorPool,
																				 m_textureIDAllocator,
																				 l_textureBatchUploadRecord))
	{
		assert(false && "テクスチャアップロード情報の作成に失敗したため、バッチテクスチャ登録に失敗しました。");
		return false;
	}

	// 作成し終えたTextureBatchUploadRecordをリストに格納する
	m_pendingTextureBatchUploadRecordMap.try_emplace(l_filePath, std::move(l_textureBatchUploadRecord));

	return true;
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

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
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