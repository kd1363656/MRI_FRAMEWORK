#pragma once

namespace FWK::Concept
{
	template <typename Type>
	concept IsDerivedTagBaseConcept = IsDerivedBaseConcept<Type, Tag::TagBase>;
}