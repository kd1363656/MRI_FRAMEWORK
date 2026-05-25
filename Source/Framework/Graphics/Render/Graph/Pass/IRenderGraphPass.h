#pragma once

namespace FWK::Graphics
{
	class IRenderGraphPass
	{
	public:

				 IRenderGraphPass() = default;
		virtual ~IRenderGraphPass() = default;

		virtual TypeAlias::TypeTag GetVALPassTag() const = 0;

		virtual const std::vector<Struct::RenderGraphTextureAccess>& GetREFTextureAccessList () const = 0;
		
		virtual void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
							 const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
							 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
							 const SwapChain&						  a_swapChain,
								   DirectCommandList&				  a_directCommandList,
								   Renderer&						  a_renderer) = 0;
	};
}