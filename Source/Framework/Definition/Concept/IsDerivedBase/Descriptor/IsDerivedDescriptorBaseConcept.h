#pragma once

namespace FWK::Concept
{
	// ディスクリプタヒープベースを継承したクラスかどうか
	template <typename Type>
	concept IsDerivedDescriptorHeapBaseConcept = IsDerivedBaseConcept<Type, Graphics::DescriptorHeapBase>;
}