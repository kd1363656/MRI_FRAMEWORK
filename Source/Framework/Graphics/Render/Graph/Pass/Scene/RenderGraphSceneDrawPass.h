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
					 const SwapChain&,
						   DirectCommandList&				  a_directCommandList,
						   Renderer&						  a_renderer,
						   RenderGraph&						  a_renderGraph) override;

		FWK_DEFINE_TYPE_INFO(RenderGraphSceneDrawPass, RenderGraphPassBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryRenderGraphPass, FWK::Graphics::RenderGraphSceneDrawPass);