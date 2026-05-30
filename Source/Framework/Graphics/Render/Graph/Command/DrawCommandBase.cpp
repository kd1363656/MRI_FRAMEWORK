#include "DrawCommandBase.h"

void FWK::Graphics::DrawCommandBase::SetupDraw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) const
{
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// MeshShaderからSRVを読むため、ShaderVisibleのSRVDescriptorHeapを設定する
	l_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	// StaticModel用ルートシグネチャとパイプラインステートをセット
	SetupGraphicsPipelineStateToCommandList(a_renderer);
}
void FWK::Graphics::DrawCommandBase::SetupPipelineStateAndRootSignature(const Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag)
{
	const auto& l_pipelineStateWeak = a_renderer.FindVALPipelineState(a_typeTag);
	const auto& l_pipelineState     = l_pipelineStateWeak.lock();

	if (!l_pipelineState) 
	{
		assert(false && "指定したTagに対応するパイプラインステートが無効です。");
		return; 
	}

	// パイプラインステートが使用するルートシグネチャを取得
	const auto& l_rootSignatureWeak = a_renderer.FindVALRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

	if (l_rootSignatureWeak.expired())
	{
		assert(false && "指定したルートシグネチャが無効です。");
		return;
	}

	m_pipelineState = l_pipelineStateWeak;
	m_rootSignature = l_rootSignatureWeak;
}

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

void FWK::Graphics::DrawCommandBase::TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Graphics::TextureRecord& a_textureRecord) const
{
	if (const auto l_currentState = a_textureRecord.GetVALCurrentState();
		l_currentState == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) { return; }

	const auto& l_textureResource = a_textureRecord.GetREFGPUResource().m_resource;

	if (!l_textureResource)
	{
		assert(false && "テクスチャリソースが無効になっており、状態遷移を行えませんでした。");
		return;
	}

	// PixelShaderからSRVとして読むため、現在の状態からPIXEL_SHADER_RESOURCEへ遷移する
	a_directCommandList.TransitionResource(a_textureRecord.GetVALCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, *l_textureResource.Get());

	a_textureRecord.SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}