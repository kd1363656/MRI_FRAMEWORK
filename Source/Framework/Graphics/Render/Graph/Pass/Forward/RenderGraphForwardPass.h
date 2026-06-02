#pragma once

namespace FWK::Graphics
{
	class RenderGraphForwardPass final : public RenderGraphPassBase
	{
	public:
		 
		 RenderGraphForwardPass();
		~RenderGraphForwardPass() override;

		void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
				     const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
					 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
					 const SwapChain&						  a_swapChain,
					 const TextureSystem&					  a_textureSystem,
						   DirectCommandList&			      a_directCommandList,
						   Renderer&						  a_renderer,
						   RenderGraph&						  a_renderGraph) override;

		FWK_DEFINE_TYPE_INFO(RenderGraphForwardPass, RenderGraphPassBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryRenderGraphPass, FWK::Graphics::RenderGraphForwardPass)