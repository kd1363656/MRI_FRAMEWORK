#pragma once

namespace FWK::Struct
{
	struct WindowCONFIG final
	{
		std::uint32_t m_width  = Constant::k_defaultWindowWidth;
		std::uint32_t m_height = Constant::k_defaultWindowHeight;

		TypeAlias::TypeTag m_styleTag = Constant::k_invalidTypeTag;
	};
}