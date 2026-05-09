#pragma once

namespace FWK::Concept
{
	// ディスクリプタヒープベースを継承したクラスかどうかを確認するコンセプト
	template <typename Type>
	concept IsDerivedIDrawCommandConcept = IsDerivedBaseConcept<Type, Graphics::IDrawCommand>;
}