#include "DrawStaticModelUnLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelUnLitStandardCommand::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetTag<Tag::ModelUnLitStandardPipelineStateTag>());
}
void FWK::Graphics::DrawStaticModelUnLitStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer)
{
}

bool FWK::Graphics::DrawStaticModelUnLitStandardCommand::SetupCBCamera(const std::weak_ptr<RootSignature>& a_rootSignature, 
																	   const Renderer&					   a_renderer,
																	   const DirectCommandList&			   a_directCommandList,
																	   const UploadBuffer&				   a_cameraUploadBuffer,
																			 std::uint8_t* const		   a_cameraMappedData) const
{
	return true;
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