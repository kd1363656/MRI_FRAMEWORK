#pragma once

namespace FWK::Graphics
{
	class DrawRequestPassBase
	{
	public:

				 DrawRequestPassBase() = default;
		virtual ~DrawRequestPassBase() = default;

		FWK_DEFINE_TYPE_INFO_ROOT(DrawRequestPassBase)
	};
}