#pragma once

namespace FWK::Utility::Math
{
	// a_valueをa_alignment倍数に切り上げる関数
	inline constexpr UINT64 AlignUp(const UINT64& a_value, const UINT64& a_alignment)
	{
		// 切り上げたい倍数が0ならreturn
		if (a_alignment == 0ULL)
		{
			return a_value;
		}

		// 余りが0なら倍数なのでa_valueをそのまま返す
		if (const auto& l_remainder = a_value % a_alignment;
			l_remainder == 0ULL)
		{
			return a_value;
		}

		// 次の倍数までa_valueを切り上げる
		return (a_value + (a_alignment - 1ULL)) & ~(a_alignment - 1ULL);
	}
}