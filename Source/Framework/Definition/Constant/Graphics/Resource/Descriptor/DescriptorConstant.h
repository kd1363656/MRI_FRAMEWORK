#pragma once

namespace FWK::Constant
{
	inline constexpr UINT k_invalidDescriptorCapacity = 0U;

	inline constexpr UINT k_defaultDescriptorCapacity  = 1024U;
	inline constexpr UINT k_invalidDescriptorHeapIndex = UINT_MAX;

	inline constexpr bool k_useCPUOnlyDescriptorHeap          = true;
	inline constexpr bool k_useShaderVisibleDescriptorHeap    = true;
	inline constexpr bool k_notUseShaderVisibleDescriptorHeap = false;
}