#include "RenderGraphDeferredLightingPass.h"

FWK::Graphics::RenderGraphDeferredLightingPass::RenderGraphDeferredLightingPass()
{
	// GBufferPassで書き込んだ三枚をPixelShaderで読む
	ReadTexture(Utility::Tag::GetVALTag<Tag::GBufferAlbedoTextureTag>(),		Utility::Tag::GetVALTag<Tag::RenderGraphShaderReadUsageTag>());
	ReadTexture(Utility::Tag::GetVALTag<Tag::GBufferNormalTextureTag>(),		Utility::Tag::GetVALTag<Tag::RenderGraphShaderReadUsageTag>());
	ReadTexture(Utility::Tag::GetVALTag<Tag::GBufferWorldPositionTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphShaderReadUsageTag>());

	// DeferredLightingの結果をSceneColorへ書き込む
	WriteTexture(Utility::Tag::GetVALTag<Tag::SceneColorTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>());
}
FWK::Graphics::RenderGraphDeferredLightingPass::~RenderGraphDeferredLightingPass() = default;

void FWK::Graphics::RenderGraphDeferredLightingPass::PostCreateSetup(Renderer& a_renderer)
{
	m_pipelineState = a_renderer.FindVALPipelineState(Utility::Tag::GetVALTag<Tag::DeferredLightingPipelineStateTag>());

	const auto& l_pipelineState = m_pipelineState.lock();

	FWK_ASSERT_RETURN_IF(!l_pipelineState, "DeferredLighting用PipelineStateが無効です。")

	m_rootSignature = a_renderer.FindVALRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

	FWK_ASSERT_RETURN_IF(m_rootSignature.expired(), "DeferredLighting用RootSignatureが無効です。")
}

void FWK::Graphics::RenderGraphDeferredLightingPass::Execute(const RTVDescriptorHeap& a_rtvDescriptorHeap, const DSVDescriptorHeap&, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, const SwapChain&, const TextureSystem&, DirectCommandList& a_directCommandList, Renderer& a_renderer, RenderGraph& a_renderGraph)
{
	// フレームリソースレコードの取得
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "FrameResourceが無効のため、DeferredLightingPassを実行できませんでした。")

	// レンダーグラフレジストリレコードの取得
	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();

	// GBuffer(アルベド)レコードを取得
	const auto& l_gBufferAlbedoTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::GBufferAlbedoTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_gBufferAlbedoTextureRecord, "GBufferAlbedoTextureが無効のため、DeferredLightingPassを実行できませんでした。")

	// GBuffer(法線)レコードを取得
	const auto& l_gBufferNormalTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::GBufferNormalTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_gBufferNormalTextureRecord, "GBufferNormalTextureが無効のため、DeferredLightingPassを実行できませんでした。")

	// GBuffer(ワールド座標)レコードを取得
	const auto& l_gBufferWorldPositionTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::GBufferWorldPositionTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_gBufferWorldPositionTextureRecord, "GBufferWorldPositionTextureが無効のため、DeferredLightingPassを実行できませんでした。")

	const auto& l_sceneColorTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetVALTag<Tag::SceneColorTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_sceneColorTextureRecord, "SceneColorTextureが無効のため、DeferredLightingPassを実行できませんでした。")

	const auto& l_gBufferAlbedoTexture        = l_gBufferAlbedoTextureRecord->m_renderTargetTexture;
	const auto& l_gBufferNormalTexture        = l_gBufferNormalTextureRecord->m_renderTargetTexture;
	const auto& l_gBufferWorldPositionTexture = l_gBufferWorldPositionTextureRecord->m_renderTargetTexture;
	const auto& l_sceneColorTexture           = l_sceneColorTextureRecord->m_renderTargetTexture;

	FWK_ASSERT_RETURN_IF(!l_gBufferAlbedoTexture,        "GBufferAlbedoTextureが無効のため、DeferredLightingPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(!l_gBufferNormalTexture,        "GBufferNormalTextureが無効のため、DeferredLightingPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(!l_gBufferWorldPositionTexture, "GBufferWorldPositionTextureが無効のため、DeferredLightingPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(!l_sceneColorTexture,           "SceneColorTextureが無効のため、DeferredLightingPassを実行できませんでした。")

	
	FWK_ASSERT_RETURN_IF(l_gBufferAlbedoTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID, "GBufferAlbedoTextureのSRVStorageIDが無効のため、DeferredLightingPassを実行できませんでした。")

	FWK_ASSERT_RETURN_IF(l_gBufferNormalTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID, "GBufferNormalTextureのSRVStorageIDが無効のため、DeferredLightingPassを実行できませんでした。")

	FWK_ASSERT_RETURN_IF(l_gBufferWorldPositionTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID, "GBufferWorldPositionTextureのSRVStorageIDが無効のため、DeferredLightingPassを実行できませんでした。")

	const auto& l_rootSignature = m_rootSignature.lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "DeferredLighting用RootSignatureが無効のため、DeferredLightingPassを実行できませんでした。")

	const auto& l_deferredLightingPassConstantBufferUploader = l_currentFrameResource->FindPTRConstantBufferUploader<DeferredLightingPassConstantBufferUploader>().lock();

	FWK_ASSERT_RETURN_IF(!l_deferredLightingPassConstantBufferUploader, "DeferredLightingPass用ConstantBufferが取得できないため、DeferredLightingPassを実行できませんでした。")

	Struct::CBDeferredLightingPass l_cbDeferredLightingPass = {};

	l_cbDeferredLightingPass.m_gBufferAlbedoTextureSRVIndex        = l_gBufferAlbedoTexture->GetVALSRVStorageID       ();
	l_cbDeferredLightingPass.m_gBufferNormalTextureSRVIndex        = l_gBufferNormalTexture->GetVALSRVStorageID       ();
	l_cbDeferredLightingPass.m_gBufferWorldPositionTextureSRVIndex = l_gBufferWorldPositionTexture->GetVALSRVStorageID();

	const auto& l_gpuVirtualAddress = l_deferredLightingPassConstantBufferUploader->Write(l_cbDeferredLightingPass);

	// RenderGraph側でGBuffer3枚はShaderResourceへ、
	// SceneColorTextureはRenderTargetへ遷移済み
	a_directCommandList.SetupRenderTargetTexture(*l_sceneColorTexture, a_rtvDescriptorHeap);

	// DeferredLightingPassがSceneColorの起点になる
	// ここでは前フレームのSceneColorを残さないためにクリアする
	a_directCommandList.ClearRenderTargetTexture(*l_sceneColorTexture, a_rtvDescriptorHeap);

	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());

	// GBufferをSRVとして読むため、SRVDescriptorHeapを設定する
	a_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	a_directCommandList.SetupRootSignature(m_rootSignature);
	a_directCommandList.SetupPipelineState(m_pipelineState);

	a_directCommandList.SetupConstantBufferView<Tag::RootParameterCBDeferredLightingPassTag>(l_gpuVirtualAddress, *l_rootSignature);

	// 全画面三角形を描画して、PixelShaderでGBufferを読む
	a_directCommandList.DispatchFullScreen();
}