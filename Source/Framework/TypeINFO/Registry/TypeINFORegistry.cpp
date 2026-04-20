#include "TypeINFORegistry.h"

void FWK::TypeINFORegistry::Register(const Struct::TypeINFO& a_typeINFO)
{
	if (a_typeINFO.k_staticTypeID == Constant::k_invalidStaticTypeID)
	{
		assert(false && "無効なIDを検出しました、型情報の確認をしてください");
		return;
	}

	if (a_typeINFO.k_name.empty())
	{
		assert(false && "文字列が空です、型情報登録の確認をしてください。");
		return;
	}

	m_allTypeINFONameMap.try_emplace        (a_typeINFO.k_name,         &a_typeINFO);
	m_allTypeINFOStaticTypeIDMap.try_emplace(a_typeINFO.k_staticTypeID, &a_typeINFO);
}

const FWK::Struct::TypeINFO* FWK::TypeINFORegistry::FindTypeINFOByID(const TypeAlias::StaticTypeID a_staticTypeID) const
{
	const auto& l_itr = m_allTypeINFOStaticTypeIDMap.find(a_staticTypeID);

	// 該当するStaticIDの型情報を取得できな蹴ればreturn;
	if (l_itr == m_allTypeINFOStaticTypeIDMap.end()) { return nullptr; }

	return l_itr->second;
}
const FWK::Struct::TypeINFO* FWK::TypeINFORegistry::FindTypeINFOByName(const std::string_view& a_name) const
{
	const auto& l_itr = m_allTypeINFONameMap.find(a_name);

	// 該当する名前の型情報を取得できな蹴ればreturn;
	if (l_itr == m_allTypeINFONameMap.end()) { return nullptr; }

	return l_itr->second;
}