#pragma once

namespace FWK::Graphics
{
	class CopyCommandQueue final : public CommandQueueBase
	{
	public:

		 CopyCommandQueue();
		~CopyCommandQueue() override;

	private:

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_COPY;
	};
}