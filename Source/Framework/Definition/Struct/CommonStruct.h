#pragma once

namespace FWK::Render
{
	class CommandAllocator;
}

namespace FWK::CommonStruct
{
	// 文字列キーをハッシュ化して高速比較するための構造体
	struct StringHash final
	{
		using is_transparent = void;

		// std::hash<T>{}(key)は、一時的に生成した関数オブジェクトを使ってハッシュ値を計算している
		std::size_t operator()(const std::string& a_key) const { return std::hash<std::string>     {}(a_key); }
		std::size_t operator()(std::string_view   a_key) const { return std::hash<std::string_view>{}(a_key); }
		std::size_t operator()(const char*        a_key) const { return std::hash<std::string_view>{}(a_key); }
	};

	struct WindowConfig final
	{
		std::uint32_t width    = CommonConstant::k_defaultWindowWidth;
		std::uint32_t height   = CommonConstant::k_defaultWindowHeight;
		TypeDEF::Tag  styleTag = CommonConstant::k_invalidStaticID;
	};
}