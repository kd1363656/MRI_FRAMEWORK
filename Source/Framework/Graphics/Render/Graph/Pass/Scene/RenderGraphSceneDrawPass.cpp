#include "RenderGraphSceneDrawPass.h"

FWK::Graphics::RenderGraphSceneDrawPass::RenderGraphSceneDrawPass()
{
	WriteTexture(Utility::Tag::GetVALTag<Tag::SceneColorTextureTag>(),		  Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>());
	WriteTexture(Utility::Tag::GetVALTag<Tag::SceneDepthStencilTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphDepthWriteUsageTag>());
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

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "FrameResourceが無効のため、SceneDrawPassを実行できませんでした。")

	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();
	const auto& l_sceneColorTextureRecord     = l_renderGraphResourceRegistry.FindVALRenderTargetTexture (Utility::Tag::GetVALTag<Tag::SceneColorTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_sceneColorTextureRecord, "SceneColorTextureが無効のため、SceneDrawPassを実行できませんでした。")
	
	const auto& l_sceneColorTexture = l_sceneColorTextureRecord->m_renderTargetTexture;

	FWK_ASSERT_RETURN_IF(!l_sceneColorTexture, "SceneColorTextureが無効のため、SceneDrawPassを実行できませんでした。")
	
	const auto& l_sceneDepthStencilTextureRecord = l_renderGraphResourceRegistry.FindVALDepthStencilTexture(Utility::Tag::GetVALTag<Tag::SceneDepthStencilTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_sceneDepthStencilTextureRecord, "SceneDepthStencilTextureが無効のため、SceneDrawPassを実行できませんでした。")
	
	const auto& l_sceneDepthStencilTexture = l_sceneDepthStencilTextureRecord->m_depthStencilTexture;

	FWK_ASSERT_RETURN_IF(!l_sceneDepthStencilTexture, "SceneDepthStencilTextureが無効のため、SceneDrawPassを実行できませんでした。")

	// RenderGraph側でSceneColorTextureはD3D12_RESOURCE_STATE_RENDER_TARGETへ、
	// SceneDepthStencilTextureはD3D12_RESOURCE_STATE_DEPTH_WRITEへ遷移済み
	// ここでは描画先としてRTV / DSVを設定する
	a_directCommandList.SetupRenderTargetTexture(*l_sceneColorTexture,
												 a_rtvDescriptorHeap,	
												 a_dsvDescriptorHeap,
												 *l_sceneDepthStencilTexture);

	// SceneDrawPassはそのフレームのシーン描画の開始地点なので、
	// 前フレームの色と深度を渡さないために明示的にクリアする
	a_directCommandList.ClearRenderTargetTexture(*l_sceneColorTexture,		  a_rtvDescriptorHeap);
	a_directCommandList.ClearDepthStencilTexture(*l_sceneDepthStencilTexture, a_dsvDescriptorHeap);
	
	// ビューポートとシザー矩形を設定する
	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());
}