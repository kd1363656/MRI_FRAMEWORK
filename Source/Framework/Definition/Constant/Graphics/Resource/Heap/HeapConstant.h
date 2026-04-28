#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::HeapPageID k_invalidHeapPageID = std::numeric_limits<TypeAlias::HeapPageID>::max();

	static constexpr UINT64 k_invalidHeapOffset = std::numeric_limits<UINT64>::max();

	static constexpr UINT64 k_invalidAllocationSize = 0ULL;
}