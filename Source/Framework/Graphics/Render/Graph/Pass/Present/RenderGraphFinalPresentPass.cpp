#include "RenderGraphFinalPresentPass.h"

FWK::Graphics::RenderGraphFinalPresentPass::RenderGraphFinalPresentPass()
{
	ReadTexture(Utility::Tag::GetTag<Tag::PostEffectColorTextureTag>(), Utility::Tag::GetTag<Tag::RenderGraphShaderReadUsageTag>());
}

FWK::Graphics::RenderGraphFinalPresentPass::~RenderGraphFinalPresentPass() = default;

void FWK::Graphics::RenderGraphFinalPresentPass::PostCreateSetup(Renderer& a_renderer)
{
	m_pipelineState = a_renderer.FindVALPipelineState(Utility::Tag::GetTag<Tag::FinalPresentPipelineStateTag>());

	const auto& l_pipelineState = m_pipelineState.lock();

	if (!l_pipelineState)
	{
		assert(false && "FinalPresent用PipelineStateが無効です。");
		return;
	}

	m_rootSignature = a_renderer.FindVALRootSignature(l_pipelineState->GetVALUseRootSignatureTag());
	
	if (m_rootSignature.expired())
	{
		assert(false && "FinalPresent用RootSignatureが無効です。");
		return;
	}
}

void FWK::Graphics::RenderGraphFinalPresentPass::Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap, 
														 const DSVDescriptorHeap&, 
														 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
													     const SwapChain&						  a_swapChain,
															   DirectCommandList&				  a_directCommandList,
															   Renderer&						  a_renderer,
															   RenderGraph&)
															   
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "FrameResourceが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_renderGraphResourceRegistry  = l_currentFrameResource->GetREFRenderGraphResourceRegistry();
	const auto& l_postEffectColorTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture (Utility::Tag::GetTag<Tag::PostEffectColorTextureTag>()).lock();

	if (!l_postEffectColorTextureRecord)
	{
		assert(false && "PostEffectColorTextureが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_postEffectColorTexture = l_postEffectColorTextureRecord->m_renderTargetTexture;

	if (!l_postEffectColorTexture)
	{
		assert(false && "PostEffectColorTextureが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	if (l_postEffectColorTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID)
	{
		assert(false && "PostEffectColorTextureのSRVStorageIDが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_rootSignature = m_rootSignature.lock();

	if (!l_rootSignature)
	{
		assert(false && "FinalPresent用RootSignatureが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_finalPresentConstantBufferUploader = l_currentFrameResource->FindPTRConstantBufferUploader<FinalPresentConstantBufferUploader>().lock();

	if (!l_finalPresentConstantBufferUploader)
	{
		assert(false && "FinalPresent用ConstantBufferが取得できないため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto&		  l_finalPresentUploadBuffer = l_finalPresentConstantBufferUploader->GetREFUploadBuffer();
		  auto* const l_finalPresentMappedData   = l_finalPresentUploadBuffer.Map						   ();

	if (!l_finalPresentMappedData)
	{
		assert(false && "FinalPresent用ConstantBufferのMapに失敗したため、FinalPresentPassを実行できませんでした。");
		return;
	}

	Struct::CBFinalPresent l_cbFinalPresent = {};

	l_cbFinalPresent.m_sceneColorTextureSRVIndex = l_postEffectColorTexture->GetVALSRVStorageID();

	const auto& l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(Struct::CBFinalPresent), Constant::k_constantBufferAlignment);
	const auto  l_constantBufferOffset	    = k_cbFinalPresentIndex * l_constantBufferAlignedSize;

	std::memcpy(l_finalPresentMappedData + l_constantBufferOffset, &l_cbFinalPresent, sizeof(Struct::CBFinalPresent));

	const auto& l_gpuVirtualAddress = l_finalPresentUploadBuffer.FetchVALGPUVirtualAddress() + l_constantBufferOffset;

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
	a_directCommandList.SetupConstantBufferView<Tag::RootParameterCBFinalPresentTag>(l_gpuVirtualAddress, *l_rootSignature);

	// PostEffectColorTextureを貼ったフルスクリーン三角形を描画する
	a_directCommandList.DispatchFullScreen();
}