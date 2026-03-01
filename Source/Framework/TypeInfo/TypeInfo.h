#pragma once

namespace FWK
{
	struct TypeInfo final
	{
		explicit TypeInfo(const FWK::TypeInfo* const a_baseInfo, const std::string_view& a_name, const std::uint32_t a_id) :
			k_baseInfo(a_baseInfo),
			k_name    (a_name),
			k_id	  (a_id)
		{}
		~TypeInfo() = default;

		const FWK::TypeInfo* const k_baseInfo;
		const std::string_view     k_name;
		const std::uint32_t	       k_id;
	};
}