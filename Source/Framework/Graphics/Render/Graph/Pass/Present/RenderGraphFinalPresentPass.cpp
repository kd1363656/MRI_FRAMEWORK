#include "RenderGraphFinalPresentPass.h"

FWK::Graphics::RenderGraphFinalPresentPass::RenderGraphFinalPresentPass()
{
	ReadTexture(Utility::Tag::GetTag<Tag::SceneColorTextureTag>(), Utility::Tag::GetTag<Tag::RenderGraphShaderReadUsageTag>());
}

FWK::Graphics::RenderGraphFinalPresentPass::~RenderGraphFinalPresentPass() = default;

void FWK::Graphics::RenderGraphFinalPresentPass::PostCreateSetup(Renderer& a_renderer)
{
	m_pipelineState = a_renderer.FindVALPipelineState(Utility::Tag::GetTag<Tag::PipelineStateTagBase>());

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
														 const DSVDescriptorHeap&				  a_dsvDescriptorHeap, 
														 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
													     const SwapChain&						  a_swapChain,
															   DirectCommandList&				  a_directCommandList,
															   Renderer&						  a_renderer)
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "FrameResourceが無効のため、FinalPresentPassを実行できませんでした。");
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

	if (l_sceneColorTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID)
	{
		assert(false && "SceneColorTextureのSRVStorageIDが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_rootSignature = m_rootSignature.lock();

	if (!l_rootSignature)
	{
		assert(false && "FinalPresent用RootSignatureが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_pipelineState = m_pipelineState.lock();

	if (!l_pipelineState)
	{
		assert(false && "FinalPresent用PipelineStateが無効のため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto& l_finalPresentConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<FinalPresentConstantBuffer>().lock();

	if (!l_finalPresentConstantBuffer)
	{
		assert(false && "FinalPresent用ConstantBufferが取得できないため、FinalPresentPassを実行できませんでした。");
		return;
	}

	const auto&		  l_finalPresentUploadBuffer = l_finalPresentConstantBuffer->GetREFUploadConstantBuffer();
		  auto* const l_finalPresentMappedData   = l_finalPresentUploadBuffer.Map						   ();

	if (!l_finalPresentMappedData)
	{
		assert(false && "FinalPresent用ConstantBufferのMapに失敗したため、FinalPresentPassを実行できませんでした。");
		return;
	}

	Struct::CBFinalPresent l_cbFinalPresent = {};

	l_cbFinalPresent.m_sceneColorTextureSRVIndex = l_sceneColorTexture->GetVALSRVStorageID();

	const auto& l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(Struct::CBFinalPresent), Constant::k_constantBufferAlignment);
	const auto  l_constantBufferOffset	    = k_cbFinalPresentIndex * l_constantBufferAlignedSize;

	std::memcpy(l_finalPresentMappedData + l_constantBufferOffset, &l_cbFinalPresent, sizeof(Struct::CBFinalPresent));

	const auto& l_gpuVirtualAddress = l_finalPresentUploadBuffer.FetchVALGPUVirtualAddress();

	// BackBufferはRenderGraphResourceRegistry管理外のSwapChainリソースなので、
	// FinalPresentPass内でPresent -> RenderTargetへ遷移する
	a_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// BackBufferを最終出力先として設定する
	a_directCommandList.SetupBackBufferRenderTarget(a_swapChain, a_rtvDescriptorHeap);

	// BackBufferを最終出力先として設定する
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

	// SceneColorTextureを貼ったフルスクリーン三角形を描画する
	a_directCommandList.DispatchFullScreenTriangle();

	// BackBufferを画面表示できる状態に戻す
	a_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}