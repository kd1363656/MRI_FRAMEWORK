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


bool FWK::Graphics::TextureSystem::LoadPendingTexturesAndWait(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	// std::unordered_set内にロードするテクスチャのファイルパスが一つもなければreturn
	if (m_pendingTextureFilePathSet.empty()) { return false; }

	// ロード申請が来ていたテクスチャを一括ロードする
	if (LoadTextureBatch(a_srvDescriptorPool))
	{
		assert(false && "ロード待ちテクスチャのバッチ登録に失敗しました。");
		return false;
	}

	// そのフレーム内でロードすべきテクスチャをすべてロードし終えた状態なのでクリア
	m_pendingTextureFilePathSet.clear();

	return false;
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::TextureSystem::LoadTextureBatch(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	if (m_pendingTextureFilePathSet.empty())
	{
		assert(false && "テクスチャ読み込み町std::unordered_setが空のため、テクスチャのバッチロード処理に失敗しました。");
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

		auto& l_textureRecord = l_textureBatchUploadRecord.m_textureRecord;

		// SRV作成用にインデックス番号を格納
		const auto l_srvIndex = a_srvDescriptorPool.Allocate();

		if (l_srvIndex == Constant::k_invalidDescriptorHeapIndex)
		{
			assert(false && "TextureIDが無効なため、バッチテクスチャ登録に失敗しました。");
			return false;
		}

		const auto l_textureID = m_textureIDAllocator.Allocate();
		
		if (l_textureID == Constant::k_invalidTextureID)
		{
			assert(false && "テクスチャIDが無効なため、バッチテクスチャ登録に失敗しました。");
			return false;
		}
		
		l_textureRecord.m_srvIndex       = l_srvIndex;
		l_textureRecord.m_textureID      = l_textureID;
		l_textureRecord.m_currentState   = D3D12_RESOURCE_STATE_COMMON;
		l_textureRecord.m_referenceCount = k_initialTextureReferenceCount;

		// CPU、GPUから編集可能なUPLOADヒープにテクスチャ情報を詰め込み
		// GPU側でしか使用できないDEFAULTヒープにUPLOADヒープの状態をコピーする


	}


	return false;
}