#pragma once

namespace FWK::Render
{
	class GraphicsCommandContext;
}

namespace FWK::Render
{
	class GraphicsCommandQueue final : public CommandQueueBase
	{
	public:

		explicit GraphicsCommandQueue(const GraphicsCommandContext& a_graphicsCommandContext);
		~GraphicsCommandQueue        ()														  override;

	private:

		const GraphicsCommandContext& k_graphicsCommandContext;
	};
}