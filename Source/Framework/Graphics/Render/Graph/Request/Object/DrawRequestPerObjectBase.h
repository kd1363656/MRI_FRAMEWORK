#pragma once

namespace FWK::Graphics	
{
	class DrawRequestPerObjectBase
	{
	public:

			     DrawRequestPerObjectBase() = default;
		virtual ~DrawRequestPerObjectBase() = default;

		FWK_DEFINE_TYPE_INFO_ROOT(DrawRequestPerObjectBase)
	};
}