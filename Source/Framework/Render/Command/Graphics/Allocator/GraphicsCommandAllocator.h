#pragma once

namespace FWK::Render
{
	class GraphicsCommandAllocator final : public CommandAllocatorBase
	{
	public:

		explicit GraphicsCommandAllocator(const GraphicsCommandContext& a_graphicsCommandContext);
		~GraphicsCommandAllocator        ()														  override;

	private:

		const GraphicsCommandContext& k_graphicsCommandContext;
	};
}