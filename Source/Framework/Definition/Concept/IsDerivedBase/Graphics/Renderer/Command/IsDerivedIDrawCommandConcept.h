#pragma once

namespace FWK::Concept
{
	// DrawCommandBaseを継承したクラスかどうかを確認するコンセプト
	template <typename Type>
	concept IsDerivedDrawCommandBaseConcept = IsDerivedBaseConcept<Type, Graphics::DrawCommandBase>;
}