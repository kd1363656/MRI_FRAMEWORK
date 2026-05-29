#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::Math::Color k_noColor = { 0.0F, 0.0F, 0.0F, 1.0F };

	inline constexpr TypeAlias::Math::Vector2 k_defaultPivot = { 0.5F, 0.5F };

	inline constexpr std::wstring_view k_defaultBaseColorTextureFilePath = L"DefaultTextureBaseColor";
	inline constexpr std::wstring_view k_defaultNormalTextureFilePath	 = L"DefaultNormal";

	inline constexpr float k_defaultDepthClearValue = 1.0F;

	inline constexpr FLOAT k_textureSRVResourceMINLODClamp = 0.0F;

	inline constexpr std::size_t k_defaultTextureCount     = static_cast<std::size_t>(Enum::DefaultTextureType::Count);
	inline constexpr std::size_t k_defaultRGBAElementCount = 4ULL;

	static constexpr std::uint32_t k_invalidTextureWidth  = 0U;
	static constexpr std::uint32_t k_invalidTextureHeight = 0U;

	inline constexpr UINT k_defaultSpriteRECTX      = 0U;
	inline constexpr UINT k_defaultSpriteRECTY      = 0U;
	inline constexpr UINT k_defaultSpriteRECTWidth  = 0U;
	inline constexpr UINT k_defaultSpriteRECTHeight = 0U;

	inline constexpr UINT k_textureSRVMostDetailedMIP = 0U;
	inline constexpr UINT k_textureSRVPlaneSlice	  = 0U;
}