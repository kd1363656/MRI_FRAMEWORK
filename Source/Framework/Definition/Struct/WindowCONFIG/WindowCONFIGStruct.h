#pragma once

namespace FWK::Struct
{
	struct WindowCONFIG
	{
		TypeAlias::TypeTag m_styleTag = Constant::k_invalidStaticTypeID;

		std::uint32_t m_width  = Constant::k_defaultWidth;
		std::uint32_t m_height = Constant::k_defaultHeight;
	};
}