#include "RenderGraphFinalPresentPass.h"

FWK::Graphics::RenderGraphFinalPresentPass::RenderGraphFinalPresentPass()
{
	ReadTexture(Utility::Tag::GetVALTag<Tag::PostEffectColorTextureTag>(), Utility::Tag::GetVALTag<Tag::RenderGraphShaderReadUsageTag>());
}

FWK::Graphics::RenderGraphFinalPresentPass::~RenderGraphFinalPresentPass() = default;

void FWK::Graphics::RenderGraphFinalPresentPass::PostCreateSetup(Renderer& a_renderer)
{
	m_pipelineState = a_renderer.FindVALPipelineState(Utility::Tag::GetVALTag<Tag::FinalPresentPipelineStateTag>());

	const auto& l_pipelineState = m_pipelineState.lock();

	FWK_ASSERT_RETURN_IF(!l_pipelineState, "FinalPresent用PipelineStateが無効です。")

	m_rootSignature = a_renderer.FindVALRootSignature(l_pipelineState->GetVALUseRootSignatureTag());
	
	FWK_ASSERT_RETURN_IF(m_rootSignature.expired(), "FinalPresent用RootSignatureが無効です。")
}

void FWK::Graphics::RenderGraphFinalPresentPass::Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap, 
														 const DSVDescriptorHeap&, 
														 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
													     const SwapChain&						  a_swapChain,
														 const TextureSystem&,
															   DirectCommandList&				  a_directCommandList,
															   Renderer&						  a_renderer,
															   RenderGraph&)
															   
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "FrameResourceが無効のため、FinalPresentPassを実行できませんでした。")

	const auto& l_renderGraphResourceRegistry  = l_currentFrameResource->GetREFRenderGraphResourceRegistry();
	const auto& l_postEffectColorTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture (Utility::Tag::GetVALTag<Tag::PostEffectColorTextureTag>()).lock();

	FWK_ASSERT_RETURN_IF(!l_postEffectColorTextureRecord, "PostEffectColorTextureが無効のため、FinalPresentPassを実行できませんでした。")

	const auto& l_postEffectColorTexture = l_postEffectColorTextureRecord->m_renderTargetTexture;

	FWK_ASSERT_RETURN_IF(!l_postEffectColorTexture,														 "PostEffectColorTextureが無効のため、FinalPresentPassを実行できませんでした。")
	FWK_ASSERT_RETURN_IF(l_postEffectColorTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID, "PostEffectColorTextureのSRVStorageIDが無効のため、FinalPresentPassを実行できませんでした。")
	
	const auto& l_rootSignature = m_rootSignature.lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "FinalPresent用RootSignatureが無効のため、FinalPresentPassを実行できませんでした。")

	const auto& l_finalPresentPassConstantBufferUploader = l_currentFrameResource->FindPTRConstantBufferUploader<FinalPresentPassConstantBufferUploader>().lock();

	FWK_ASSERT_RETURN_IF(!l_finalPresentPassConstantBufferUploader, "FinalPresent用ConstantBufferが取得できないため、FinalPresentPassを実行できませんでした。")

	Struct::CBFinalPresentPass l_cbFinalPresentPass = {};

	l_cbFinalPresentPass.m_sceneColorTextureSRVIndex = l_postEffectColorTexture->GetVALSRVStorageID();

	const auto& l_gpuVirtualAddress = l_finalPresentPassConstantBufferUploader->Write(l_cbFinalPresentPass);

	// BackBufferはRenderGraphResourceRegistry管理外のSwapChainリソースなので、
	// FinalPresentPass内でPresent -> RenderTargetへ遷移する
	a_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// BackBufferを最終出力先として設定する
	a_directCommandList.SetupBackBufferRenderTarget(a_swapChain, a_rtvDescriptorHeap);

	// BackBufferへ最終結果を書き込むため、前フレームの内容を残さないようにクリアする
	a_directCommandList.ClearBackBufferRenderTarget(a_swapChain, a_rtvDescriptorHeap);

	// ビューポートとシザー矩形を設定する
	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());

	// FinalPresent用のSRVDescriptorHeapを設定する
	a_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	// FinalPresent用RootSignature / PipelineStateを設定する
	a_directCommandList.SetupRootSignature(m_rootSignature);
	a_directCommandList.SetupPipelineState(m_pipelineState);

	// SetupConstantBufferView(設定するGPU仮想アドレス、
	//						   ルートパラメータを探すRootSignature);
	// FinalPresent用CBをb0へ設定する
	a_directCommandList.SetupConstantBufferView<Tag::RootParameterCBFinalPresentPassTag>(l_gpuVirtualAddress, *l_rootSignature);

	// PostEffectColorTextureを貼ったフルスクリーン三角形を描画する
	a_directCommandList.DispatchFullScreen();
}