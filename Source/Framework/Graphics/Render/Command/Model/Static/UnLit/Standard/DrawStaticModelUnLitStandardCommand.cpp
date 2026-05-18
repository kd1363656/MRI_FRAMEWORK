#include "DrawStaticModelUnLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelUnLitStandardCommand::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetTag<Tag::ModelUnLitStandardPipelineStateTag>());
}
void FWK::Graphics::DrawStaticModelUnLitStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer)
{
	// StaticModel用ルートシグネチャとパイプラインステートをセット
	SetupGraphicsPipelineStateToCommandList(a_renderer);

	if (const auto& l_rootSignature = GetVALRootSignature();
		l_rootSignature.expired())
	{
		assert(false && "使用仕様としたルートシグネチャが無効なため、StaticModel描画処理に失敗しました。");
		return;
	}

	const auto& l_currentFrameResource = a_renderer.FetchVALCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "現在のフレームリソースの取得に失敗しました。");
		return;
	}
}

bool FWK::Graphics::DrawStaticModelUnLitStandardCommand::SetupCBModelObject(const std::weak_ptr<RootSignature>&				   a_rootSignature,
																			const Struct::StaticModelUnLitStandardDrawCommand& a_staticModelUnLitStandardDrawCommand, 
																			const DirectCommandList&						   a_directCommandList, 
																			const UploadBuffer&								   a_modelObjectUploadBuffer, 
																			const std::size_t&								   a_modelObjectIndex, 
																				  std::uint8_t* const						   a_modelObjectMappedData) const
{
	return false;
}

bool FWK::Graphics::DrawStaticModelUnLitStandardCommand::SetupModelMeshStructuredBufferSRV(const std::weak_ptr<RootSignature>&	    a_rootSignature, 
																						   const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
																						   const DirectCommandList&					a_directCommandList,
																						   const Struct::ModelMeshRuntimeData&	    a_modelMeshRuntimeData) const
{
	return false;
}