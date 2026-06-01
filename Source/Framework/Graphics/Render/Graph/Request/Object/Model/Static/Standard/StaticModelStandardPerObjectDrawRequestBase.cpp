#include "StaticModelStandardPerObjectDrawRequestBase.h"

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::BeginFrame()
{
	m_forwardPerObjectDataList.BeginFrame();
}

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::AddForwardDrawRequestData(const std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestData)
{
	m_forwardPerObjectDataList.AddDrawRequestPerObject(a_staticModelStandardPerObjectDrawRequestData);
}

bool FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::SetupModelMeshConstantBuffer(const RootSignature&										 a_rootSignature, 
																							  const DirectCommandList&									 a_directCommandList,
																							  const FrameResource&										 a_frameResource,
																							  const Struct::StaticModelStandardPerObjectDrawRequestData& a_staticModelStandardPerObjectDrawRequestData, 
																							  const Struct::ModelMesh&									 a_modelMesh)
{

}

bool FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::TransitionMaterialTexture(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh, const bool a_isUseLightPass) const
{
	return false;
}

bool FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::DispatchModelMesh(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh) const
{
	return false;
}