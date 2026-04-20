#pragma once

namespace FWK::Concept
{
	// TagBase(ルート基底)を継承しているかどうかを確認する
	template <typename Type>
	concept IsDerivedTagBaseConcept = IsDerivedBaseConcept<Type, Tag::TagBase>;
}