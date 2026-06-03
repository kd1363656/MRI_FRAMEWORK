#include "RenderGraphGBufferPass.h"

FWK::Graphics::RenderGraphGBufferPass::RenderGraphGBufferPass()
{
	// DeferredRendering用のGBufferを書き込む
	WriteTexture(Utility::Tag::GetVALTag<Tag::GBufferAlbedoTextureTag>(),		 Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>());
	WriteTexture(Utility::Tag::GetVALTag<Tag::GBufferNormalTextureTag>(),		 Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>());
	WriteTexture(Utility::Tag::GetVALTag<Tag::GBufferWorldPositionTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>());

	// GBufferPassではDepthを書き込む
	WriteTexture(Utility::Tag::GetVALTag<Tag::SceneDepthStencilTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphDepthWriteUsageTag>());
}
FWK::Graphics::RenderGraphGBufferPass::~RenderGraphGBufferPass() = default;

void FWK::Graphics::RenderGraphGBufferPass::Execute(const RTVDescriptorHeap&			     a_rtvDescriptorHeap, 
												    const DSVDescriptorHeap&			     a_dsvDescriptorHeap, 
													const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
												    const SwapChain&						 a_swapChain, 
													const TextureSystem&				     a_textureSystem, 
														  DirectCommandList&				 a_directCommandList,
														  Renderer&							 a_renderer, 
														  RenderGraph&						 a_renderGraph)
{
	// フレームリソースの取得
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "FrameResourceが無効のため、GBufferPassを実行できませんでした。")

	// レンダーグラフレジストリーの取得
	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();

	// GBuffer(アルベドを取得)
	const auto& l_gBufferAlbedoTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::GBufferAlbedoTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_gBufferAlbedoTextureRecord, "GBufferAlbedoTextureが無効のため、GBufferPassを実行できませんでした。")

	// GBuffer(ノーマルを取得)
	const auto& l_gBufferNormalTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::GBufferNormalTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_gBufferNormalTextureRecord, "GBufferNormalTextureが無効のため、GBufferPassを実行できませんでした。")

	const auto& l_gBufferWorldPositionTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::GBufferWorldPositionTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_gBufferWorldPositionTextureRecord, "GBufferWorldPositionTextureが無効のため、GBufferPassを実行できませんでした。")

	const auto& l_sceneDepthStencilTextureRecord = l_renderGraphResourceRegistry.FindVALDepthStencilTexture(Utility::Tag::GetVALTag<Tag::SceneDepthStencilTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_sceneDepthStencilTextureRecord, "SceneDepthStencilTextureが無効のため、GBufferを実行できませんでした。")

	const auto& l_gBufferAlbedoTexture		  = l_gBufferAlbedoTextureRecord->m_renderTargetTexture;
	const auto& l_gBufferNormalTexture		  = l_gBufferNormalTextureRecord->m_renderTargetTexture;
	const auto& l_gBufferWorldPositionTexture = l_gBufferWorldPositionTextureRecord->m_renderTargetTexture;
	const auto& l_sceneDepthStencilTexture    = l_sceneDepthStencilTextureRecord->m_depthStencilTexture;

	FWK_ASSERT_RETURN_IF(!l_gBufferAlbedoTexture,		 "GBufferAlbedoTextureが無効のため、GBufferPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(!l_gBufferNormalTexture,		 "GBufferNormalTextureが無効のため、GBufferPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(!l_gBufferWorldPositionTexture, "GBufferWorldPositionTextureが無効のため、GBufferPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(!l_sceneDepthStencilTexture,    "SceneDepthStencilTextureが無効のため、GBufferPassを実行できませんでした。")

	const std::vector<std::reference_wrapper<const RenderTargetTexture>> l_gBufferRenderTargetTextureList = 
	{
		std::cref(*l_gBufferAlbedoTexture),
		std::cref(*l_gBufferNormalTexture),
		std::cref(*l_gBufferWorldPositionTexture),
	};

	// GBuffer3枚 + DepthStencilをOMステージに設定する
	a_directCommandList.SetupRenderTargetTexture(l_gBufferRenderTargetTextureList,
												 a_rtvDescriptorHeap,
												 a_dsvDescriptorHeap,
												 *l_sceneDepthStencilTexture);

	// GBufferは毎フレーム作り直す情報なので、前フレームの値を渡さない
	a_directCommandList.ClearRenderTargetTexture(*l_gBufferAlbedoTexture,        a_rtvDescriptorHeap);
	a_directCommandList.ClearRenderTargetTexture(*l_gBufferNormalTexture,        a_rtvDescriptorHeap);
	a_directCommandList.ClearRenderTargetTexture(*l_gBufferWorldPositionTexture, a_rtvDescriptorHeap);
	a_directCommandList.ClearDepthStencilTexture(*l_sceneDepthStencilTexture,    a_dsvDescriptorHeap);

	// 画面に描画する範囲を設定する
	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());

	// 後でGBuffer描画時にMaterialTextureやStructuredBufferをSRVとして読むため、ここで設定しておく
	a_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	if (const auto& l_staticModelStandardPerObjectDrawRequestLit = a_renderGraph.FindVALDrawRequestPerObject<StaticModelStandardPerObjectDrawRequestLit>().lock())
	{
		l_staticModelStandardPerObjectDrawRequestLit->RequestDeferredDraw(a_textureSystem, a_renderer);
	}
}