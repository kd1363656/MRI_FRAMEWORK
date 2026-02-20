#pragma once

namespace FWK::CommonConstant
{
	inline constexpr float k_defaultFPS = 60.0F;

	inline constexpr std::uint32_t k_invalidStaticID = 0U;

	inline constexpr std::uint32_t k_defaultWindowWidth  = 1280U;
	inline constexpr std::uint32_t k_defaultWindowHeight = 720U;

	inline constexpr std::uint64_t k_initialFenceValue = 0ULL;

	inline constexpr UINT k_defaultGPUNodeMask   = 0U;
	inline constexpr UINT k_defaultBackBufferNum = 2U;
	inline constexpr UINT k_defaultSyncInterval  = 1U;
}