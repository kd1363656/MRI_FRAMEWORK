#pragma once

namespace FWK::Utility::Tag
{
	template <Concept::IsDerivedTagBaseConcept Type>
	inline auto GetTag()
	{
		return Type::GetTypeINFO().k_staticTypeID;
	}
}