#pragma once

namespace FWK::Graphics
{
	class DirectCommandQueue final : public CommandQueueBase
	{
	public:

		 DirectCommandQueue();
		~DirectCommandQueue() override;

	private:

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	};
}