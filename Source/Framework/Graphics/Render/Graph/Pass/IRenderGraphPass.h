#pragma once

namespace FWK::Graphics
{
	class Renderer;
	class RenderGraph;
}

namespace FWK::Graphics
{
	class IRenderGraphPass
	{
	public:

				 IRenderGraphPass() = default;
		virtual ~IRenderGraphPass() = default;

		virtual const std::vector<Struct::RenderGraphTextureAccess>& GetREFTextureAccessList () const = 0;
		
		virtual void PostCreateSetup(Renderer&) {};

		virtual void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
							 const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
							 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
							 const SwapChain&						  a_swapChain,
								   DirectCommandList&				  a_directCommandList,
								   Renderer&						  a_renderer,
								   RenderGraph&						  a_renderGraph) = 0;

		FWK_DEFINE_TYPE_INFO_ROOT(IRenderGraphPass);
	};
}