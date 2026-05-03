#pragma once

namespace FWK::Concept
{
	template <typename Type>
	concept IsDerivedPipelineStateTagBaseConcept = IsDerivedBaseConcept<Type, Tag::PipelineStateTagBase>;
}