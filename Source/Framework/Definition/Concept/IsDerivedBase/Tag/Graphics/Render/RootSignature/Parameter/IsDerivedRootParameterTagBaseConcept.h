#pragma once

namespace FWK::Concept
{
	template <typename Type>
	concept IsDerivedRootParameterTagBaseConcept = IsDerivedBaseConcept<Type, Tag::RootParameterTagBase>;	
}