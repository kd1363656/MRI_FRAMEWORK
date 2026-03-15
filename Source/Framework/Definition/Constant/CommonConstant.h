#pragma once

namespace FWK::CommonConstant
{
	inline constexpr TextureID k_invalidTextureID = 0U;

	inline constexpr TextureHeapPageID k_invalidTextureHeapPageID = UINT_MAX;

	// フェンス値野初期値が0の時、デバックレイヤーが計画を出すため
	inline constexpr UINT64 k_initialFenceValue = 1ULL;
	inline constexpr UINT64 k_unusedFenceValue  = 0ULL;

	inline constexpr UINT k_defaultBackBufferNum = 2U;

	inline constexpr UINT k_defaultSyncInterval = 1U;

	inline constexpr UINT k_invalidDescriptorHeapIndex = UINT_MAX;

	inline constexpr UINT k_defaultDescriptorNum = 1024U;

	inline constexpr std::uint32_t k_invalidStaticID = 0U;

	inline constexpr std::uint32_t k_invalidTag = 0U;
	
	// 一般的な解像度の幅と高さ
	inline constexpr std::uint32_t k_defaultWindowWidth  = 1280U;
	inline constexpr std::uint32_t k_defaultWindowHeight = 720U;

	inline constexpr float k_defaultFPS = 60.0F;

	inline const std::filesystem::path k_lowerJsonExtension = ".json";
}