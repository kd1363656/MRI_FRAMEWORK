#pragma once

namespace FWK::Struct
{
	struct WindowSize final
	{
		std::uint32_t m_width  = Constant::k_defaultWindowWidth;
		std::uint32_t m_height = Constant::k_defaultWindowHeight;

	};

	struct WindowCONFIG final
	{
		WindowSize m_clientSize = {};

		TypeAlias::TypeTag m_styleTag = Constant::k_invalidTypeTag;
	};

	struct WindowResizeRequest final
	{
		WindowSize m_clientSize = {};

		bool m_isRequested = false;
		bool m_isMinimized = false;
	};
}