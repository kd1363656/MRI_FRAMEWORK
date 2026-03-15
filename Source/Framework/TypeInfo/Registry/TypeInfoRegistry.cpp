#include "TypeInfoRegistry.h"

void FWK::TypeInfoRegistry::Register(const CommonStruct::TypeInfo& a_typeInfo)
{
	if (a_typeInfo.k_id == CommonConstant::k_invalidStaticID)
	{
		assert(false && "無効なIDを検出しました、型情報の確認をしてください");
		return;
	}

	if (a_typeInfo.k_name.empty())
	{
		assert(false && "文字列が空です、型情報登録の確認をしてください。");
		return;
	}

	m_allTypeInfoIDMap.try_emplace(a_typeInfo.k_id, &a_typeInfo);
	m_allTypeInfoNameMap.try_emplace(a_typeInfo.k_name, &a_typeInfo);
}

const FWK::CommonStruct::TypeInfo* FWK::TypeInfoRegistry::FindTypeInfoByID(const std::uint32_t a_id) const
{
	const auto& l_itr = m_allTypeInfoIDMap.find(a_id);

	if (l_itr == m_allTypeInfoIDMap.end())
	{
		return nullptr;
	}

	return l_itr->second;
}
const FWK::CommonStruct::TypeInfo* FWK::TypeInfoRegistry::FindTypeInfoByName(const std::string_view& a_name) const
{
	const auto& l_itr = m_allTypeInfoNameMap.find(a_name);

	if (l_itr == m_allTypeInfoNameMap.end())
	{
		return nullptr;
	}

	return l_itr->second;
}