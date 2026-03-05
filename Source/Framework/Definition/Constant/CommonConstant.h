#pragma once

namespace FWK::CommonConstant
{
	inline const std::string k_extensionDDS  = ".dds";
	inline const std::string k_extensionTGA  = ".tga";
	inline const std::string k_extensionPNG  = ".png";
	inline const std::string k_extensionJPG  = ".jpg";
	inline const std::string k_extensionJPEG = ".jpeg";
	inline const std::string k_extensionBMP  = ".bmp";

	inline constexpr float k_defaultFPS = 60.0F;

	inline constexpr std::uint32_t k_invalidStaticID = 0U;

	inline constexpr std::uint32_t k_defaultWindowWidth  = 1280U;
	inline constexpr std::uint32_t k_defaultWindowHeight = 720U;

	// 0だとデバックレイヤーでエラーが出るから
	inline constexpr UINT64 k_initialFenceValue = 1ULL;

	inline constexpr UINT k_defaultGPUNodeMask   = 0U;
	inline constexpr UINT k_defaultBackBufferNum = 2U;
	inline constexpr UINT k_defaultSyncInterval  = 1U;

	inline constexpr UINT k_invalidTextureID = 0U;
}