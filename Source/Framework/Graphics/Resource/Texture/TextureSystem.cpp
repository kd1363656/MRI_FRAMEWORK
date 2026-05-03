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

	// 既に登録済みのテクスチャなら再度ロード申請する必要がないのでreturn
	if (const auto& l_itr = m_texturePathMap.find(l_filePath);
		l_itr != m_texturePathMap.end())
	{
		return true;
	}

	// 現在のフレームで登録しようとしているパスが既に登録されているなら登録する必要がないためreturn
	if (const auto& l_itr = m_pendingTextureFilePathSet.find(l_filePath);
		l_itr != m_pendingTextureFilePathSet.end())
	{
		return true;
	}	

	// 現在のフレームでロードするテクスチャのファイルパスをstd::unordered_setに格納
	m_pendingTextureFilePathSet.emplace(l_filePath);
	
	return true;
}


void FWK::Graphics::TextureSystem::LoadPendingTexturesAndWait(const Device&			                   a_device, 
															  const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																	DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
																	UploadSystem&					   a_uploadSystem)
{
	// std::unordered_set内にロードするテクスチャのファイルパスが一つもなければreturn
	if (m_pendingTextureFilePathSet.empty()) { return; }

	// ロード申請が来ていたテクスチャを一括ロードする
	if (!LoadTextureBatch(a_device, 
						 a_gpuMemoryAllocator,
						 a_srvDescriptorPool,
						 a_uploadSystem))
	{
		assert(false && "ロード待ちテクスチャのバッチ登録に失敗しました。");
		return;
	}

	// そのフレーム内でロードすべきテクスチャをすべてロードし終えた状態なのでクリア
	m_pendingTextureFilePathSet.clear();
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::TextureSystem::LoadTextureBatch(const Device&			                 a_device, 
													const GPUMemoryAllocator&                a_gpuMemoryAllocator,
														  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
														  UploadSystem&						 a_uploadSystem)
{
	if (m_pendingTextureFilePathSet.empty())
	{
		assert(false && "テクスチャ読み込み待ちstd::unordered_setが空のため、テクスチャのバッチロード処理に失敗しました。");
		return false;
	}

	// テクスチャアップロード用データと実際に保存するTextureRecordの情報を持ったリスト
	std::vector<Struct::TextureBatchUploadRecord> l_textureBatchUploadRecordList = {};

	l_textureBatchUploadRecordList.reserve(m_pendingTextureFilePathSet.size());

	// このフレームでロード申請されたテクスチャのファイルパスを使用してテクスチャをロードしていく
	for (const auto& l_pendingTextureFilePath : m_pendingTextureFilePathSet)
	{
		if (l_pendingTextureFilePath.empty()) { continue; }

		if (const auto& l_itr = m_texturePathMap.find(l_pendingTextureFilePath);
			l_itr != m_texturePathMap.end())
		{
			assert(false && "登録済みテクスチャがバッチロード処理に含まれていたため、テクスチャのバッチロード処理に失敗しました。");
			return false;
		}

		DirectX::ScratchImage l_scratchImage = {};
		DirectX::TexMetadata  l_texMetadata  = {};

		// まずはテクスチャをロードしてロードできるかどうかを確認
		if (!m_textureLoader.LoadTextureFile(l_scratchImage, l_texMetadata, l_pendingTextureFilePath))
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
																					 l_pendingTextureFilePath,
																					 a_srvDescriptorPool,
																					 m_textureIDAllocator,
																					 l_textureBatchUploadRecord))
		{
			assert(false && "テクスチャアップロード情報の作成に失敗したため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		// 作成し終えたTextureBatchUploadRecordをリストに格納する
		l_textureBatchUploadRecordList.emplace_back(std::move(l_textureBatchUploadRecord));
	}

	if (!a_uploadSystem.SubmitTextureCopyBatchAndWait(l_textureBatchUploadRecordList))
	{
		assert(false && "UploadSystemでのバッチテクスチャコピーに失敗したため、バッチテクスチャ登録に失敗しました。。");
		return false;
	}

	for (auto& l_record : l_textureBatchUploadRecordList)
	{
		auto& l_textureRecord = l_record.m_textureRecord;

		m_texturePathMap.try_emplace  (l_record.m_filePath,			l_textureRecord.m_textureID);
		m_textureRecordMap.try_emplace(l_textureRecord.m_textureID, std::move(l_record.m_textureRecord));
	}

	return true;
}