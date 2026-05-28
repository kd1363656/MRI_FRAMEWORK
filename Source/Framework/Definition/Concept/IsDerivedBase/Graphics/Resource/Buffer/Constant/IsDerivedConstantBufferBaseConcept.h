#pragma once

namespace FWK::Concept
{
	// ディスクリプタヒープベースを継承したクラスかどうかを確認するコンセプト
	template <typename Type>
	concept IsDerivedConstantBufferUploaderBaseConcept = IsDerivedBaseConcept<Type, Graphics::ConstantBufferUploaderBase>;
}