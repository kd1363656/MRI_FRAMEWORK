#pragma once

namespace FWK::Utility::Math
{
	inline constexpr UINT64 k_emptyAlignment      = 0ULL;
	inline constexpr UINT64 k_emptyRemainder      = 0ULL;
	inline constexpr UINT64 k_alignmentMaskOffset = 1ULL;

	// a_valueをa_alignment倍数に切り上げる関数
	inline constexpr UINT64 AlignUp(const UINT64& a_value, const UINT64& a_alignment)
	{
		// 切り上げたい倍数が0ならreturn
		if (a_alignment == k_emptyAlignment)
		{
			return a_value;
		}

		// 余りが0なら倍数なのでa_valueをそのまま返す
		if (const auto& l_remainder = a_value % a_alignment;
			l_remainder == k_emptyRemainder)
		{
			return a_value;
		}

		// 次の倍数までa_valueを切り上げる
		return (a_value + (a_alignment - k_alignmentMaskOffset)) & ~(a_alignment - k_alignmentMaskOffset);
	}
}