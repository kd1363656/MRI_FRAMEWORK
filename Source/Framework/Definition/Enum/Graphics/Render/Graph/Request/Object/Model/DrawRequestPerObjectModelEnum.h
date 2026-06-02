#pragma once

namespace FWK::Enum
{
	enum class StaticModelRenderingPath : std::uint32_t
	{
		Forward,
		Deferred,
		Count,
	};
}