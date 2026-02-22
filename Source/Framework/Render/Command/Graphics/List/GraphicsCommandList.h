#pragma once

namespace FWK::Render
{
	class GraphicsCommandList final : public CommandListBase
	{
	public:

		explicit GraphicsCommandList(const GraphicsCommandContext& a_graphicsCommandContext);
		~GraphicsCommandList        () override;

	private:

		const GraphicsCommandContext& k_graphicsCommandContext;
	};
}