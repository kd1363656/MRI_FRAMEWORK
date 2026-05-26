#pragma once

namespace FWK::Graphics
{
	class RenderGraphSceneDrawPass final : public RenderGraphPassBase
	{
	public:
		
		 RenderGraphSceneDrawPass();
		~RenderGraphSceneDrawPass() override;

		void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
					 const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
					 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
					 const SwapChain&						  a_swapChain,
						   DirectCommandList&				  a_directCommandList,
						   Renderer&						  a_renderer) override;

		FWK_DEFINE_TYPE_INFO(RenderGraphSceneDrawPass, RenderGraphPassBase);
	};
}