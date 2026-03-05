#pragma once

namespace FWK::Utility::Math
{
	// a_valueをa_alignment倍数に切り上げる関数
	inline UINT64 AlignUp(const UINT64& a_value, const UINT64& a_alignment)
	{
		return (a_value + (a_alignment - 1ULL)) & ~(a_alignment - 1ULL);
	}
}