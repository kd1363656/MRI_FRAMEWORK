#include "RenderGraphPostEffectPass.h"

FWK::Graphics::RenderGraphPostEffectPass::RenderGraphPostEffectPass()
{
	ReadTexture (Utility::Tag::GetTag<Tag::SceneColorTextureTag>(),		 Utility::Tag::GetTag<Tag::RenderGraphShaderReadUsageTag>());
	WriteTexture(Utility::Tag::GetTag<Tag::PostEffectColorTextureTag>(), Utility::Tag::GetTag<Tag::RenderGraphRenderTargetUsageTag>());
}
FWK::Graphics::RenderGraphPostEffectPass::~RenderGraphPostEffectPass() = default;

void FWK::Graphics::RenderGraphPostEffectPass::PostCreateSetup(Renderer& a_renderer)
{
	m_pipelineState = a_renderer.FindVALPipelineState(Utility::Tag::GetTag<Tag::PostEffectPipelineStateTag>());

	const auto& l_pipelineState = m_pipelineState.lock();

	if (!l_pipelineState)
	{
		assert(false && "PostEffect用PipelineStateが無効です。");
		return;
	}

	m_rootSignature = a_renderer.FindVALRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

	if (m_rootSignature.expired())
	{
		assert(false && "PostEffect用RootSignatureが無効です。");
		return;
	}
}

void FWK::Graphics::RenderGraphPostEffectPass::Execute(const RTVDescriptorHeap&				    a_rtvDescriptorHeap, 
													   const DSVDescriptorHeap&,
													   const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
													   const SwapChain&, 
															 DirectCommandList&					a_directCommandList,
															 Renderer&							a_renderer, 
															 RenderGraph&)
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "FrameResourceが無効のため、PostEffectPassを実行できませんでした。");
		return;
	}

	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();
	const auto& l_sceneColorTextureRecord	  = l_renderGraphResourceRegistry.FindVALRenderTargetTexture (Utility::Tag::GetTag<Tag::SceneColorTextureTag>()).lock();

	// シーンカラーテクスチャの取得
	if (!l_sceneColorTextureRecord)
	{
		assert(false && "SceneColorTextureが無効のため、PostEffectPassを実行できませんでした。");
		return;
	}

	const auto& l_sceneColorTexture = l_sceneColorTextureRecord->m_renderTargetTexture;

	if (!l_sceneColorTexture)
	{
		assert(false && "SceneColorTextureが無効のため、PostEffectPassを実行できませんでした。。");
		return;
	}

	if (l_sceneColorTexture->GetVALSRVStorageID() == Constant::k_invalidStorageID)
	{
		assert(false && "SceneColorTextureのSRVStorageIDが無効のため、PostEffectPassを実行できませんでした。");
		return;
	}

	// ポストエフェクトカラーテクスチャの取得
	const auto& l_postEffectColorTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetTag<Tag::PostEffectColorTextureTag>()).lock();

	if (!l_postEffectColorTextureRecord)
	{
		assert(false && "PostEffectColorTextureが無効のため、PostEffectPassを実行できませんでした。");
		return;
	}

	const auto& l_postEffectColorTexture = l_postEffectColorTextureRecord->m_renderTargetTexture;

	if (!l_postEffectColorTexture)
	{
		assert(false && "PostEffectColorTextureが無効のため、PostEffectPassを実行できませんでした。");
		return;
	}

	const auto& l_rootSignature = m_rootSignature.lock();

	if (!l_rootSignature)
	{
		assert(false && "PostEffect用RootSignatureが無効のため、PostEffectPassを実行できませんでした。");
		return;
	}

	const auto& l_postEffectConstantBufferUploader = l_currentFrameResource->FindPTRConstantBufferUploader<PostEffectConstantBufferUploader>().lock();

	if (!l_postEffectConstantBufferUploader)
	{
		assert(false && "PostEffect用ConstantBufferが取得できないため、PostEffectPassを実行できませんでした。");
		return;
	}

	const auto&		  l_postEffectUploadBuffer = l_postEffectConstantBufferUploader->GetREFUploadBuffer();
		  auto* const l_postEffectMappedData   = l_postEffectUploadBuffer.Map						   ();

	if (!l_postEffectMappedData)
	{
		assert(false && "PostEffect用ConstantBufferのMapに失敗したため、PostEffectPassを実行できませんでした。");
		return;
	}

	Struct::CBPostEffect l_cbPostEffect = {};

	l_cbPostEffect.m_sourceTextureSRVIndex = l_sceneColorTexture->GetVALSRVStorageID();

	const auto& l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(Struct::CBPostEffect), Constant::k_constantBufferAlignment);
	const auto  l_constantBufferOffset		= k_cbPostEffectIndex * l_constantBufferAlignedSize;

	std::memcpy(l_postEffectMappedData + l_constantBufferOffset, &l_cbPostEffect, sizeof(Struct::CBPostEffect));

	const auto& l_gpuVirtualAddress = l_postEffectUploadBuffer.FetchVALGPUVirtualAddress() + l_constantBufferOffset;

	// RenderGraph側でSceneColorTextureはShaderResourceへ、
	// PostEffectColorTextureはRenderTargetへ遷移済み
	// ここではPostEffectColorTextureを描画先として設定する
	a_directCommandList.SetupRenderTargetTexture(*l_postEffectColorTexture, a_rtvDescriptorHeap);

	// PostEffectColorTextureへ新しく書き込むため、前フレームの結果を残さないようにクリアする
	a_directCommandList.ClearRenderTargetTexture(*l_postEffectColorTexture, a_rtvDescriptorHeap);

	// ビューポートとシザー矩形を設定する
	a_directCommandList.SetupRenderArea(a_renderer.GetREFRenderArea());

	// PostEffect用のSRVDescriptorHeapを設定する
	a_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	// PostEffect用RootSignature/PipelineStateを設定する
	a_directCommandList.SetupRootSignature(m_rootSignature);
	a_directCommandList.SetupPipelineState(m_pipelineState);

	// SetupConstantBufferView(設定するGPU仮想アドレス、
	//						   ルートパラメータを探すRootSignature);
	// PostEffect用CBを設定する
	a_directCommandList.SetupConstantBufferView<Tag::RootParameterCBPostEffectTag>(l_gpuVirtualAddress, *l_rootSignature);

	// SceneColorTextureを読んでPostEffectColorTextureへ書き込む
	// DispatchMeshはMeshShaderを実行するDirectX12関数
	// 今回は1グループだけ実行し、MeshShader側で画面全体を覆う三角形を1枚生成する
	a_directCommandList.DispatchFullScreen();
}