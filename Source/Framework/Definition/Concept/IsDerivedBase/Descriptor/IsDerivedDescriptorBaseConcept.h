#pragma once

namespace FWK::Concept
{
	template <typename Type>
	concept IsDerivedDescriptorHeapBase = IsDerivedBaseConcept<Type, Graphics::DescriptorHeapBase>;
}