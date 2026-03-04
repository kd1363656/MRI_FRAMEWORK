#pragma once

namespace FWK::Graphics
{
	class CopyCommandAllocator final : public CommandAllocatorBase
	{
	public:

		explicit CopyCommandAllocator(const Device& a_device);
		~CopyCommandAllocator        ()						  override;
	};
}