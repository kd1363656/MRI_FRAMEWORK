#pragma once

namespace FWK::Concept
{
	// ディスクリプタヒープベースを継承したクラスかどうかを確認するコンセプト
	template <typename Type>
	concept IsDerivedConstantBufferBaseConcept = IsDerivedBaseConcept<Type, Graphics::ConstantBufferBase>;
}