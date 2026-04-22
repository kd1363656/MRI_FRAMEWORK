#pragma once

namespace FWK::Concept
{
	template <typename Type>
	concept IsDerivedDescriptorHeapBaseConcept = IsDerivedBaseConcept<Type, Graphics::DescriptorHeapBase>;
}