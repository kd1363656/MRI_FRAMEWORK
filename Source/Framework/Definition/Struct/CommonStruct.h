#pragma once

namespace FWK::CommonStruct
{
	// 文字列キー用の透過ハッシュ構造体
	struct StringHash final
	{
		// 透過ハッシュであることをSTLに通知するための宣言
		using is_transparent = void;

		// 関数オペレーターを呼び出すことで与えられた値をハッシュ値に変換する
		std::size_t operator()(const std::string& a_key) const { return std::hash<std::string>     {}(a_key); }
		std::size_t operator()(std::string_view   a_key) const { return std::hash<std::string_view>{}(a_key); }
		std::size_t operator()(const char*        a_key) const { return std::hash<std::string_view>{}(a_key); }
	};

	struct WindowConfig final
	{
		std::uint32_t width    = CommonConstant::k_defaultWindowWidth;
		std::uint32_t height   = CommonConstant::k_defaultWindowHeight;
		::Tag		  styleTag = CommonConstant::k_invalidStaticID;
	};

	struct TextureRecord
	{
		ComPtr<ID3D12Resource2> resource;

		std::string sourcePath = std::string();

		TextureID textureID = CommonConstant::k_invalidTextureID;
		
		// メタ情報
		UINT        srvIndex = 0U;
		UINT		width    = 0U;
		UINT        height   = 0U;
		DXGI_FORMAT format   = DXGI_FORMAT_UNKNOWN;
	};
}