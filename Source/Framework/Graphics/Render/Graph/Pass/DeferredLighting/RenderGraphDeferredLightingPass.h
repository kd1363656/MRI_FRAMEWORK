#pragma once

namespace FWK::Graphics
{
	class RenderGraphDeferredLightingPass final : public RenderGraphPassBase
	{
	public:

		 RenderGraphDeferredLightingPass();
		~RenderGraphDeferredLightingPass() override;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
					 const DSVDescriptorHeap&,
					 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
					 const SwapChain&,
					 const TextureSystem&,
					 	   DirectCommandList&				  a_directCommandList,
					 	   Renderer&						  a_renderer,
					 	   RenderGraph&						  a_renderGraph) override;
		
	private:

		std::weak_ptr<RootSignature> m_rootSignature = {};
		std::weak_ptr<PipelineState> m_pipelineState = {};

		FWK_DEFINE_TYPE_INFO(RenderGraphDeferredLightingPass, RenderGraphPassBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryRenderGraphPass, FWK::Graphics::RenderGraphDeferredLightingPass)