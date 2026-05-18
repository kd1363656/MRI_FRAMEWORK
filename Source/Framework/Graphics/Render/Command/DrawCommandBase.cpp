#include "DrawCommandBase.h"

void FWK::Graphics::DrawCommandBase::SetupGraphicsPipelineStateToCommandList(Renderer& a_renderer) const
{
	if (m_pipelineState.expired()) 
	{
		assert(false && "使用するパイプラインステートが作成されておらず、描画を開始できませんでした。");
		return;
	}

	if (m_rootSignature.expired())
	{
		assert(false && "使用するルートシグネチャが作成されておらず、描画を開始できませんでした。");
		return;
	}

	auto& l_directCommandList = a_renderer.GetMutableREFDirectCommandList();

	// ルートシグネチャをセット
	l_directCommandList.SetupRootSignature(m_rootSignature);

	// パイプラインステートをセット
	l_directCommandList.SetupPipelineState(m_pipelineState);
}

void FWK::Graphics::DrawCommandBase::TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Struct::TextureRecord& a_textureRecord)
{
	if (a_textureRecord.m_currentState == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) { return; }

	// PixelShaderからSRVとして読むため、現在の状態からPIXEL_SHADER_RESOURCEへ遷移する
	a_directCommandList.TransitionResource(a_textureRecord.m_gpuResource.m_resource,
										   a_textureRecord.m_currentState,
										   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	a_textureRecord.m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}