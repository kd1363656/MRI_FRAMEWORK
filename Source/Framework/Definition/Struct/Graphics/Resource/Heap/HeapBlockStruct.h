#pragma once

namespace FWK::Struct
{
	struct FreeHeapBlock final
	{
		UINT64 beginOffset = 0ULL;
		UINT64 size	       = 0ULL;
	};
}