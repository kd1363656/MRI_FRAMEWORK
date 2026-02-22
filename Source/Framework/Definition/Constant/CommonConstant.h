#pragma once

namespace FWK::CommonConstant
{
	inline constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListTypeDirect = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	inline constexpr D3D12_DESCRIPTOR_HEAP_TYPE k_descriptorHeapTypeRTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	inline constexpr float k_defaultFPS = 60.0F;

	inline constexpr std::uint32_t k_invalidStaticID = 0U;

	inline constexpr std::uint32_t k_defaultWindowWidth  = 1280U;
	inline constexpr std::uint32_t k_defaultWindowHeight = 720U;

	inline constexpr std::uint64_t k_initialFenceValue = 0ULL;

	inline constexpr UINT k_defaultGPUNodeMask   = 0U;
	inline constexpr UINT k_defaultBackBufferNum = 2U;
	inline constexpr UINT k_defaultSyncInterval  = 1U;
}