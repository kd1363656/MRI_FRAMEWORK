#pragma once

namespace FWK::Struct
{
	struct FreeHeapBlock final
	{
		UINT64 m_beginOffset = 0ULL;
		UINT64 m_size	     = 0ULL;
	};

	struct PlacedResourceAllocationRecord final
	{
		TypeAlias::HeapPageID m_heapPageID = Constant::k_invalidHeapPageID;

		UINT64 m_heapOffset     = 0ULL;
		UINT64 m_allocationSize = 0ULL;
	};
}