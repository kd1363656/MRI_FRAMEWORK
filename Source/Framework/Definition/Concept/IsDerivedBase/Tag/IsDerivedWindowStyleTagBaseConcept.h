#pragma once

namespace FWK::Concept
{
	template <typename Type>
	concept IsDerivedWindowStyleTagBaseConcept = IsDerivedBaseConcept<Type, Tag::WindowStyleTagBase>;
}