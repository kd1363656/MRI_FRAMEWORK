#include "StaticModelStandardPerObjectDrawRequestUnLit.h"

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestUnLit::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetVALTag<Tag::ModelUnLitStandardPipelineStateTag>());
}

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestUnLit::RequestForwardDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer)
{
	const auto& l_renderGraph       = a_renderer.GetMutableREFRenderGraph();
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList ();
	const auto& l_rootSignature     = GetVALRootSignature			     ().lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "使用しようとしたルートシグネチャが無効なため、StaticModel描画処理に失敗しました。")

	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "現在のフレームリソースの取得に失敗しました。")

	const auto& l_cameraPassDrawRequest = l_renderGraph.FindVALDrawRequestPass<CameraPassDrawRequest>().lock();
	
	FWK_ASSERT_RETURN_IF(!l_cameraPassDrawRequest, "カメラパスのポインタが無効です。")
	
	FWK_ASSERT_RETURN_IF(!l_cameraPassDrawRequest->SetupCommonPassConstantBuffer(*l_rootSignature, l_directCommandList, *l_currentFrameResource), "カメラパスの設定に失敗しました。")

	const auto& l_drawRequestList = GetREFForwardPerObjectDataList();

	SetupModelMeshConstantBuffer(*l_rootSignature,
								 l_directCommandList,
								 *l_currentFrameResource,
								 l_drawRequestList,
								 a_textureSystem);
}