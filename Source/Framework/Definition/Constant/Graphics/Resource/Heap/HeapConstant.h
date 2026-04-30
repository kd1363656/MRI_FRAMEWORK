#pragma once

namespace FWK::Constant
{
	inline constexpr UINT64 k_invalidHeapOffset     = std::numeric_limits<UINT64>::max();
	inline constexpr UINT64 k_invalidAllocationSize = 0ULL;
	inline constexpr UINT64 k_invalidAlignment      = 0ULL;

	inline constexpr TypeAlias::HeapPageID k_invalidHeapPageID = std::numeric_limits<TypeAlias::HeapPageID>::max();
}