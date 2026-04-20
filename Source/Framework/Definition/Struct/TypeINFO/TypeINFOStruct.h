#pragma once

namespace FWK::Struct
{
	// 型情報を扱う構造体
	struct TypeINFO final
	{
		explicit TypeINFO(const TypeINFO* const a_baseINFO, const TypeAlias::StaticTypeID a_staticTypeID, const std::string_view& a_name) :
			k_baseINFO    (a_baseINFO),
			k_staticTypeID(a_staticTypeID),
			k_name        (a_name)
		{}
		~TypeINFO() = default;

		const TypeINFO* const         k_baseINFO;
		const TypeAlias::StaticTypeID k_staticTypeID;
		const std::string_view        k_name;
	};
}