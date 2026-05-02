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