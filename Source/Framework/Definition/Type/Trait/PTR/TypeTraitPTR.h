#pragma once

namespace FWK::TypeTrait
{
	//==============================================================
	// スマートポインタ判定
	//==============================================================
	template <typename Type>
	struct PTRType
	{
		static constexpr Enum::PTRKind k_kind = Enum::PTRKind::None;
	};
	
	//==============================================================
	// 生ポインタ判定
	//==============================================================
	template <typename Type>
	struct PTRType<Type*>
	{
		static constexpr Enum::PTRKind k_kind = Enum::PTRKind::Raw;
	};

	template <typename Type>
	struct PTRType<std::shared_ptr<Type>>
	{
		static constexpr Enum::PTRKind k_kind = Enum::PTRKind::Shared;
	};
	
	template <typename Type>
	struct PTRType<std::unique_ptr<Type>>
	{
		static constexpr Enum::PTRKind k_kind = Enum::PTRKind::Unique;
	};
}