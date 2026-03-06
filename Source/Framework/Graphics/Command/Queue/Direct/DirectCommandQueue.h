#pragma once

namespace FWK::Graphics
{
	class DirectCommandQueue final : public CommandQueueBase
	{
	public:

		explicit DirectCommandQueue(const Device& a_device);
		~DirectCommandQueue        ()					    override;
	};
}