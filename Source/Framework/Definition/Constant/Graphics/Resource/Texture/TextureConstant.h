#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::Math::Color k_noColor = { 0.0F, 0.0F, 0.0F, 1.0F };

	inline constexpr TypeAlias::Math::Vector2 k_defaultPivot = { 0.5F, 0.5F };

	inline constexpr std::wstring_view k_defaultBaseColorTextureFilePath = L"DefaultTextureBaseColor";
	inline constexpr std::wstring_view k_defaultNormalTextureFilePath	 = L"DefaultNormal";

	inline constexpr float k_defaultDepthClearValue = 1.0F;

	inline constexpr std::size_t k_defaultTextureCount     = static_cast<std::size_t>(Enum::DefaultTextureType::Count);
	inline constexpr std::size_t k_defaultRGBAElementCount = 4ULL;

	inline constexpr UINT k_defaultSpriteRECTX      = 0U;
	inline constexpr UINT k_defaultSpriteRECTY      = 0U;
	inline constexpr UINT k_defaultSpriteRECTWidth  = 0U;
	inline constexpr UINT k_defaultSpriteRECTHeight = 0U;

	inline constexpr UINT k_renderTextureDefaultSampleCount   = 1U;
	inline constexpr UINT k_renderTextureDefaultSampleQuality = 0U;

	inline constexpr UINT16 k_renderTextureDefaultArraySize = 1U;
	inline constexpr UINT16 k_renderTextureDefaultMIPLevels = 1U;

	inline constexpr std::uint8_t k_defaultTextureWhiteChannel = 255U;
	inline constexpr std::uint8_t k_defaultTextureAlphaChannel = 255U;

	inline constexpr std::uint8_t k_defaultTextureNormalXChannel = 128U;
	inline constexpr std::uint8_t k_defaultTextureNormalYChannel = 128U;
	inline constexpr std::uint8_t k_defaultTextureNormalZChannel = 255U;

	inline constexpr UINT8 k_defaultStencilClearValue = 0U;
}