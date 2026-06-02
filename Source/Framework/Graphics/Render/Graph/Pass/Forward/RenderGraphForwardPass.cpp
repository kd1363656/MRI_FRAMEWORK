#include "RenderGraphForwardPass.h"

FWK::Graphics::RenderGraphForwardPass::RenderGraphForwardPass()
{
	// ForwardPassはSceneColorTextureへ通常描画する。
	// RenderGraphはこの宣言を見て、SceneColorTextureをRenderTarget状態へ遷移してくれる
	WriteTexture(Utility::Tag::GetVALTag<Tag::SceneColorTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>());

	// ForwardPassではDepthを書き込む。
	// 現段階ではSceneDrawPassの置き換えなのでDepthWriteにしておく。にしておく
	// 後でDeferredLightingPassの後ろに置く場合は、半透明用にDepthReadへ変える可能性がある。
	WriteTexture(Utility::Tag::GetVALTag<Tag::SceneDepthStencilTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphDepthWriteUsageTag>());
}
FWK::Graphics::RenderGraphForwardPass::~RenderGraphForwardPass() = default;

void FWK::Graphics::RenderGraphForwardPass::Execute(const RTVDescriptorHeap&				 a_rtvDescriptorHeap,
													const DSVDescriptorHeap&				 a_dsvDescriptorHeap, 
													const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
												    const SwapChain&, 
													const TextureSystem&					 a_textureSystem,
														  DirectCommandList&				 a_directCommandList,
														  Renderer&							 a_renderer, 
														  RenderGraph&						 a_renderGraph)
{
	// フレームリソースを取得
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "FrameResourceが無効のため、ForwardPassを実行できませんでした。")

	// レンダーグラフレジストリとシーンカラーテクスチャレコードを取得
	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();
	const auto& l_sceneColorTextureRecord     = l_renderGraphResourceRegistry.FindVALRenderTargetTexture (Utility::Tag::GetVALTag<Tag::SceneColorTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_sceneColorTextureRecord, "SceneColorTextureが無効のため、ForwardPassを実行できませんでした。")

	// シーンカラーテクスチャを取得
	const auto& l_sceneColorTexture = l_sceneColorTextureRecord->m_renderTargetTexture;

	FWK_ASSERT_RETURN_IF(!l_sceneColorTexture, "SceneColorTextureが無効のため、ForwardPassを実行できませんでした。")

	// シーンデプスステンシルレコードを取得
	const auto& l_sceneDepthStencilTextureRecord = l_renderGraphResourceRegistry.FindVALDepthStencilTexture(Utility::Tag::GetVALTag<Tag::SceneDepthStencilTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_sceneDepthStencilTextureRecord, "SceneDepthStencilTextureが無効のため、ForwardPassを実行できませんでした。")

	// シーンデプスステンシルテクスチャを取得
	const auto& l_sceneDepthStencilTexture = l_sceneDepthStencilTextureRecord->m_depthStencilTexture;

	FWK_ASSERT_RETURN_IF(!l_sceneDepthStencilTexture, "SceneDepthStencilTextureが無効のため、ForwardPassを実行できませんでした。")

	// RenderGraph側でSceneColorTextureはD3D12_RESOURCE_STATE_RENDER_TARGETへ、
	// SceneDepthStencilTextureはD3D12_RESOURCE_STATE_DEPTH_WRITEへ遷移済み
	// ここでは実際に描画先としてRTV/DSVを設定する
	a_directCommandList.SetupRenderTargetTexture(*l_sceneColorTexture,
												 a_rtvDescriptorHeap,
												 a_dsvDescriptorHeap,
												 *l_sceneDepthStencilTexture);

	// ForwardPassは現段階ではSceneDrawPassの置き換えなのでクリアする。
	// 後でDeferredLightingPassの後ろに置く場合、SceneColorはクリアせずに上書き/合成する設計に使える
	a_directCommandList.ClearRenderTargetTexture(*l_sceneColorTexture,		  a_rtvDescriptorHeap);
	a_directCommandList.ClearDepthStencilTexture(*l_sceneDepthStencilTexture, a_dsvDescriptorHeap);

	// ビューポートとシザー矩形を設定する
	// これを設定しないと、描画範囲が正しくならない。
	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());

	// SRVDescriptorHeapをForwardPass内で一回だけ設定
	// DescriptorHeapの切り替えは比較的重いため、DrawRequestごとに毎回設定しない
	a_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	if (const auto& l_staticModelStandardPerObjectDrawRequestLit = a_renderGraph.FindVALDrawRequestPerObject<StaticModelStandardPerObjectDrawRequestLit>().lock())
	{
		l_staticModelStandardPerObjectDrawRequestLit->SetupBeforeDrawRequest(a_renderer);

		// LitモデルをForwardで描画する。
		// CameraPass / LightPassの定数バッファ設定はRequestForwardDraw内部で行う。
		l_staticModelStandardPerObjectDrawRequestLit->RequestForwardDraw(a_textureSystem, a_renderer);
	}

	if (const auto& l_staticModelStandardPerObjectDrawRequestUnLit = a_renderGraph.FindVALDrawRequestPerObject<StaticModelStandardPerObjectDrawRequestUnLit>().lock())
	{
		l_staticModelStandardPerObjectDrawRequestUnLit->SetupBeforeDrawRequest(a_renderer);

		// UnLitモデルをForwardで描画する。
		// ライト計算をしないモデルや、デバッグ表示などに使う
		l_staticModelStandardPerObjectDrawRequestUnLit->RequestForwardDraw(a_textureSystem, a_renderer);
	}
}