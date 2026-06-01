#include "DrawRequestPerObjectBase.h"

void FWK::Graphics::DrawRequestPerObjectBase::SetupPipelineStateAndRootSignature(const Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag)
{
	const auto& l_pipelineStateWeak = a_renderer.FindVALPipelineState(a_typeTag);
	const auto& l_pipelineState     = l_pipelineStateWeak.lock();

	FWK_ASSERT_RETURN_IF(!l_pipelineState, "指定したTagに対応するパイプラインステートが無効です。")

	// パイプラインステートが使用するルートシグネチャを取得
	const auto& l_rootSignatureWeak = a_renderer.FindVALRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

	FWK_ASSERT_RETURN_IF(l_rootSignatureWeak.expired(), "指定したルートシグネチャが無効です。")

	m_pipelineState = l_pipelineStateWeak;
	m_rootSignature = l_rootSignatureWeak;
}

void FWK::Graphics::DrawRequestPerObjectBase::SetupGraphicsPipelineStateToCommandList(Renderer& a_renderer) const
{
	FWK_ASSERT_RETURN_IF(m_pipelineState.expired(), "使用するパイプラインステートが作成されておらず、描画を開始できませんでした。")
	FWK_ASSERT_RETURN_IF(m_rootSignature.expired(), "使用するルートシグネチャが作成されておらず、描画を開始できませんでした。")

	auto& l_directCommandList = a_renderer.GetMutableREFDirectCommandList();

	// ルートシグネチャをセット
	l_directCommandList.SetupRootSignature(m_rootSignature);

	// パイプラインステートをセット
	l_directCommandList.SetupPipelineState(m_pipelineState);
}

void FWK::Graphics::DrawRequestPerObjectBase::TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Graphics::TextureRecord& a_textureRecord) const
{
	if (const auto l_currentState = a_textureRecord.GetVALCurrentState();
		l_currentState == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) 
	{
		return; 
	}

	const auto& l_textureResource = a_textureRecord.GetREFGPUResource().m_resource;

	FWK_ASSERT_RETURN_IF(!l_textureResource, "テクスチャリソースが無効になっており、状態遷移を行えませんでした。")

	// PixelShaderからSRVとして読むため、現在の状態からPIXEL_SHADER_RESOURCEへ遷移する
	a_directCommandList.TransitionResource(a_textureRecord.GetVALCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, *l_textureResource.Get());

	a_textureRecord.SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}