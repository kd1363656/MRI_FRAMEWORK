#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::HeapPageID k_invalidHeapPageID = std::numeric_limits<TypeAlias::HeapPageID>::max();

	inline constexpr UINT64 k_unusedFreeHeapBlockBeginOffset = 0ULL;
}