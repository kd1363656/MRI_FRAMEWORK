#pragma once

namespace FWK::Render
{
	class GraphicsCommandQueue : public CommandQueueBase
	{
	public:

		GraphicsCommandQueue ();
		~GraphicsCommandQueue();

	private:

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;

	};
}