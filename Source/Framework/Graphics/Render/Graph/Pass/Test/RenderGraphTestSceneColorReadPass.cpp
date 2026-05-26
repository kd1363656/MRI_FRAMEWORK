#include "RenderGraphTestSceneColorReadPass.h"

FWK::Graphics::RenderGraphTestSceneColorReadPass::RenderGraphTestSceneColorReadPass()
{
	ReadTexture(Utility::Tag::GetTag<Tag::SceneColorTextureTag>(), Utility::Tag::GetTag<Tag::RenderGraphCopySourceUsageTag>());
}
FWK::Graphics::RenderGraphTestSceneColorReadPass::~RenderGraphTestSceneColorReadPass() = default;

void FWK::Graphics::RenderGraphTestSceneColorReadPass::Execute(const RTVDescriptorHeap& a_rtvDescriptorHeap, const DSVDescriptorHeap& a_dsvDescriptorHeap, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, const SwapChain& a_swapChain, DirectCommandList& a_directCommandList, Renderer& a_renderer)
{

}