#pragma once
#pragma once

namespace FWK
{
	struct TypeInfo final
	{
		explicit TypeInfo(const FWK::TypeInfo* const a_baseInfo,
						  const std::string_view&    a_name,
						  const std::uint32_t        a_id,
						  const bool                 a_isRegisterd = false) :
			k_baseInfo   (a_baseInfo),
			k_name       (a_name),
			k_id	     (a_id),
			k_isRegisterd(a_isRegisterd)
		{}
		~TypeInfo() = default;

		const FWK::TypeInfo* const k_baseInfo;
		const std::string_view     k_name;
		const std::uint32_t	       k_id;
		const bool				   k_isRegisterd;
	};
}