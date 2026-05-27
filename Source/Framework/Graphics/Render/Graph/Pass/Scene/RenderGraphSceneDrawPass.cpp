#include "RenderGraphSceneDrawPass.h"

FWK::Graphics::RenderGraphSceneDrawPass::RenderGraphSceneDrawPass()
{
	WriteTexture(Utility::Tag::GetTag<Tag::SceneColorTextureTag>(),		   Utility::Tag::GetTag<Tag::RenderGraphRenderTargetUsageTag>());
	WriteTexture(Utility::Tag::GetTag<Tag::SceneDepthStencilTextureTag>(), Utility::Tag::GetTag<Tag::RenderGraphDepthWriteUsageTag>());
}
FWK::Graphics::RenderGraphSceneDrawPass::~RenderGraphSceneDrawPass() = default;

void FWK::Graphics::RenderGraphSceneDrawPass::Execute(const RTVDescriptorHeap&				   a_rtvDescriptorHeap, 
													  const DSVDescriptorHeap&				   a_dsvDescriptorHeap, 
													  const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
													  const SwapChain&, 
															DirectCommandList&				   a_directCommandList, 
															Renderer&						   a_renderer,
															RenderGraph&					   a_renderGraph)
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "FrameResourceが無効のため、SceneDrawPassを実行できませんでした。");
		return;
	}

	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();
	const auto& l_sceneColorTextureRecord     = l_renderGraphResourceRegistry.FindVALRenderTargetTexture (Utility::Tag::GetTag<Tag::SceneColorTextureTag>()).lock();

	if (!l_sceneColorTextureRecord)
	{
		assert(false && "SceneColorTextureが無効のため、SceneDrawPassを実行できませんでした。");
		return;
	}

	const auto& l_sceneColorTexture = l_sceneColorTextureRecord->m_renderTargetTexture;

	if (!l_sceneColorTexture)
	{
		assert(false && "SceneColorTextureが無効のため、SceneDrawPassを実行できませんでした。");
		return;
	}

	const auto& l_sceneDepthStencilTextureRecord = l_renderGraphResourceRegistry.FindVALDepthStencilTexture(Utility::Tag::GetTag<Tag::SceneDepthStencilTextureTag>()).lock();

	if (!l_sceneDepthStencilTextureRecord)
	{
		assert(false && "SceneDepthStencilTextureが無効のため、SceneDrawPassを実行できませんでした。");
		return;
	}

	const auto& l_sceneDepthStencilTexture = l_sceneDepthStencilTextureRecord->m_depthStencilTexture;

	if (!l_sceneDepthStencilTexture)
	{
		assert(false && "SceneDepthStencilTextureが無効のため、SceneDrawPassを実行できませんでした。");
		return;
	}

	// RenderGraph側でSceneColorTextureはD3D12_RESOURCE_STATE_RENDER_TARGETへ、
	// SceneDepthStencilTextureはD3D12_RESOURCE_STATE_DEPTH_WRITEへ遷移済み
	// ここでは描画先としてRTV / DSVを設定する
	a_directCommandList.SetupRenderTargetTexture(*l_sceneColorTexture,
												 a_rtvDescriptorHeap,	
												 a_dsvDescriptorHeap,
												 *l_sceneDepthStencilTexture);

	// ビューポートとシザー矩形を設定する
	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());

	// 通常の描画コマンドを実行する
	a_renderGraph.Draw(a_srvDescriptorPool, a_renderer);
}