#pragma once

namespace FWK::Graphics
{
	class DirectCommandAllocator final : public CommandAllocatorBase
	{
	public:

		explicit DirectCommandAllocator(const Device& a_device);
		~DirectCommandAllocator        () override;
	};
}