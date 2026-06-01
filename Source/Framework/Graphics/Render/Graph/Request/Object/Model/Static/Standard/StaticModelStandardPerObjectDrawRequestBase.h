#pragma once

namespace FWK::Graphics
{
	class StaticModelStandardPerObjectDrawRequestBase : public DrawRequestPerObjectBase
	{
	public:

		 StaticModelStandardPerObjectDrawRequestBase()			= default;
		~StaticModelStandardPerObjectDrawRequestBase() override = default;

		void BeginFrame() override;

		virtual void RequestForwardDraw(Renderer& a_renderer) = 0;

		void AddForwardDrawRequestData(const std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestData);

	protected:

		const auto& GetREFForwardPerObjectDataList() const { return m_forwardPerObjectDataList; }

		bool SetupModelMeshConstantBuffer(const RootSignature&										 a_rootSignature,
										  const DirectCommandList&									 a_directCommandList,
										  const FrameResource&										 a_frameResource,
										  const Struct::StaticModelStandardPerObjectDrawRequestData& a_staticModelStandardPerObjectDrawRequestData,
										  const Struct::ModelMesh&									 a_modelMesh);

		bool TransitionMaterialTexture(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh, const bool a_isUseLightPass) const;

		bool DispatchModelMesh(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh) const;

	private:

		DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData> m_forwardPerObjectDataList = {};

		FWK_DEFINE_TYPE_INFO(StaticModelStandardPerObjectDrawRequestBase, DrawRequestPerObjectBase)
	};
}