#include "TypeInfoRegistry.h"

void FWK::TypeInfoRegistry::Register(const FWK::TypeInfo& a_info)
{
	if (a_info.k_id == FWK::CommonConstant::k_invalidStaticID)
	{
		assert(false && "–³Œø‚È\"ID\"‚ðŒŸoAŒ^î•ñ“o˜^‚ÌŠm”F‚ð‚µ‚Ä‚­‚¾‚³‚¢");
		return;
	}

	if (a_info.k_name.empty())
	{
		assert(false && "•¶Žš—ñ‚ª‹ó‚Å‚·AŒ^î•ñ“o˜^‚ÌŠm”F‚ð‚µ‚Ä‚­‚¾‚³‚¢");
		return;
	}

	m_allTypeInfoIDMap.try_emplace  (a_info.k_id,   &a_info);
	m_allTypeInfoNameMap.try_emplace(a_info.k_name, &a_info);
}

const FWK::TypeInfo* FWK::TypeInfoRegistry::FetchTypeInfoByID(const std::uint32_t a_id) const
{
	auto l_itr = m_allTypeInfoIDMap.find(a_id);

	if (l_itr == m_allTypeInfoIDMap.end())
	{
		return nullptr;
	}

	return l_itr->second;
}
const FWK::TypeInfo* FWK::TypeInfoRegistry::FetchTypeInfoByName(const std::string_view& a_name) const
{
	auto l_itr = m_allTypeInfoNameMap.find(a_name);

	if (l_itr == m_allTypeInfoNameMap.end())
	{
		return nullptr;
	}

	return l_itr->second;
}