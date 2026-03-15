#pragma once

namespace FWK::Utility::Tag
{
	template <Concept::IsDerivedTagBaseConcept Type>
	inline std::uint32_t GetTag()
	{
		return Type::GetTypeInfo().k_id;
	}
}