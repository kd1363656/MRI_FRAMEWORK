#include "DrawCommandBase.h"

void FWK::Graphics::DrawCommandBase::SetupPipelineStateAndRootSignature(Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag)
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

void FWK::Graphics::DrawCommandBase::TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Struct::TextureRecord& a_textureRecord)
{
	if (a_textureRecord.m_currentState == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) { return; }

	// PixelShaderからSRVとして読むため、現在の状態からPIXEL_SHADER_RESOURCEへ遷移する
	a_directCommandList.TransitionResource(a_textureRecord.m_gpuResource.m_resource,
										   a_textureRecord.m_currentState,
										   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	a_textureRecord.m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

bool FWK::Graphics::DrawCommandBase::SetCBCamera(const std::weak_ptr<RootSignature>& a_rootSignature, 
												 const std::weak_ptr<Camera>&		 a_camera, 
												 const DirectCommandList&		     a_directCommandList,
												 const UploadBuffer&				 a_cameraUploadBuffer, 
													   std::uint8_t* const			 a_cameraMappedData) const
{
	const auto& l_camera = a_camera.lock();

	if (!l_camera)
	{
		assert(false && "Cameraが無効なため、Camera用定数バッファの指定に失敗しました。");
		return false;
	}

	const auto& l_cbCamera = l_camera->CreateCBCamera();

	return SetupConstantBuffer<Tag::RootParameterCBCameraTag>(a_rootSignature,
															  a_directCommandList,	
															  a_cameraUploadBuffer,
															  l_cbCamera,
															  k_cbCameraIndex,
															  a_cameraMappedData);
}