#pragma once

namespace FWK::Graphics
{
	class CopyCommandQueue final : public CommandQueueBase
	{
	public:

		explicit CopyCommandQueue(const Device& a_device);
		~CopyCommandQueue        ()						  override;
	};
}