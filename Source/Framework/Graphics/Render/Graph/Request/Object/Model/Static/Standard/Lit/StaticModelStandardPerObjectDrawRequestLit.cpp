#include "StaticModelStandardPerObjectDrawRequestLit.h"

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestLit::RequestForwardDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer)
{
	const auto& l_pipelineStateTag = Utility::Tag::GetVALTag<Tag::ModelLitStandardPipelineStateTag>();
	const auto& l_rootSignature    = SetGraphicsPipelineStateAndFetchRootSignature				   (a_renderer, l_pipelineStateTag).lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "Forward描画用RootSignatureが無効なため、StaticModel描画処理に失敗しました。")

	const auto& l_renderGraph       = a_renderer.GetMutableREFRenderGraph();
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList ();

	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "現在のフレームリソースの取得に失敗しました。")

	const auto& l_cameraPassDrawRequest = l_renderGraph.FindVALDrawRequestPass<CameraPassDrawRequest>().lock();
	const auto& l_lightPassDrawRequest  = l_renderGraph.FindVALDrawRequestPass<LightPassDrawRequest> ().lock();
	
	FWK_ASSERT_RETURN_IF(!l_cameraPassDrawRequest, "カメラパスのポインタが無効です。")
	FWK_ASSERT_RETURN_IF(!l_lightPassDrawRequest,  "ライトパスのポインタが無効です。")

	FWK_ASSERT_RETURN_IF(!l_cameraPassDrawRequest->SetupCommonPassConstantBuffer(*l_rootSignature, l_directCommandList, *l_currentFrameResource), "カメラパスの設定に失敗しました。")
	FWK_ASSERT_RETURN_IF(!l_lightPassDrawRequest->SetupCommonPassConstantBuffer (*l_rootSignature, l_directCommandList, *l_currentFrameResource), "ライトパスの設定に失敗しました。")

	const auto& l_drawRequestList = GetREFForwardPerObjectDataList();

	SetupModelMeshConstantBuffer(*l_rootSignature,
								 l_directCommandList,
								 *l_currentFrameResource,
								 l_drawRequestList,
								 a_textureSystem);
}
void FWK::Graphics::StaticModelStandardPerObjectDrawRequestLit::RequestDeferredDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer)
{
	const auto& l_pipelineStateTag = Utility::Tag::GetVALTag<Tag::ModelLitGBufferPipelineStateTag>();
	const auto& l_rootSignature    = SetGraphicsPipelineStateAndFetchRootSignature				  (a_renderer, l_pipelineStateTag).lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "Forward描画用RootSignatureが無効なため、StaticModel描画処理に失敗しました。")

	const auto& l_renderGraph       = a_renderer.GetMutableREFRenderGraph();
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList ();

	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "現在のフレームリソースの取得に失敗しました。")

	const auto& l_cameraPassDrawRequest = l_renderGraph.FindVALDrawRequestPass<CameraPassDrawRequest>().lock();
	
	FWK_ASSERT_RETURN_IF(!l_cameraPassDrawRequest,																								  "カメラパスのポインタが無効です。")
	FWK_ASSERT_RETURN_IF(!l_cameraPassDrawRequest->SetupCommonPassConstantBuffer(*l_rootSignature, l_directCommandList, *l_currentFrameResource), "カメラパスの設定に失敗しました。")

	const auto& l_drawRequestList = GetREFDeferredPerObjectDataList();

	SetupModelMeshConstantBuffer(*l_rootSignature,
								 l_directCommandList,
								 *l_currentFrameResource,
								 l_drawRequestList,
								 a_textureSystem);
}