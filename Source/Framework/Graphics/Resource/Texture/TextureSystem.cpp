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


bool FWK::Graphics::TextureSystem::LoadPendingTexturesAndWait()
{
	// std::unordered_set内にロードするテクスチャのファイルパスが一つもなければreturn
	if (m_pendingTextureFilePathSet.empty()) { return false; }

	// ロード申請が来ていたテクスチャを一括ロードする
	if (LoadTextureBatch())
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

bool FWK::Graphics::TextureSystem::LoadTextureBatch()
{
	if (m_pendingTextureFilePathSet.empty())
	{
		assert(false && "テクスチャ読み込み町std::unordered_setが空のため、テクスチャのバッチロード処理に失敗しました。");
		return false;
	}



	return false;
}