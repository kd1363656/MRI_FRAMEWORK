#pragma once

namespace FWK::Constant
{
	inline constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_cbvSRVUAVDescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	inline constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_rtvDescriptorHeapType       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	
	inline constexpr UINT k_invalidDescriptorCapacity = 0U;

	inline constexpr UINT k_defaultDescriptorCapacity  = 1024U;
	inline constexpr UINT k_invalidDescriptorHeapIndex = UINT_MAX;

	inline constexpr bool k_useCPUOnlyDescriptorHeap          = true;
	inline constexpr bool k_useShaderVisibleDescriptorHeap    = true;
	inline constexpr bool k_notUseShaderVisibleDescriptorHeap = false;
}