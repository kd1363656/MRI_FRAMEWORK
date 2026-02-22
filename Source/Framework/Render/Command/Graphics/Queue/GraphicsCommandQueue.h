#pragma once

namespace FWK::Render
{
	class GraphicsCommandContext;
}

namespace FWK::Render
{
	class GraphicsCommandQueue : public CommandQueueBase
	{
	public:
		explicit GraphicsCommandQueue(const GraphicsCommandContext& a_graphicsCommandContext, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		~GraphicsCommandQueue        ()														 override;

	private:

		const GraphicsCommandContext& k_graphicsCommandContext;
	};
}