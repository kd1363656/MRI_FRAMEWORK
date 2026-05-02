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

	// 既に登録済みのテクスチャなら再度ロード申請する必要はない
	if (const auto& l_itr = m_texturePathMap.find(l_filePath);
		l_itr != m_texturePathMap.end())
	{
		return true;
	}

	return true;
}

nlohmann::json FWK::Graphics::TextureSystem::Serialize() const
{
	return m_textureSystemJsonConverter.Serialize(*this);
}