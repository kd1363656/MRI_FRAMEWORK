#pragma once

namespace FWK::Graphics
{
	class StaticModelStandardPerObjectDrawRequestBase : public DrawRequestPerObjectBase
	{
	public:

		 StaticModelStandardPerObjectDrawRequestBase()			= default;
		~StaticModelStandardPerObjectDrawRequestBase() override = default;

		void BeginFrame() override;

		virtual void RequestForwardDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer) = 0;

		void AddForwardDrawRequestData(const std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestData);

	protected:

		const auto& GetREFForwardPerObjectDataList() const { return m_forwardPerObjectDataList; }

		void SetupModelMeshConstantBuffer(const RootSignature&																   a_rootSignature,
										  const DirectCommandList&															   a_directCommandList,
										  const FrameResource&																   a_frameResource,
										  const DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestDataList,
										  const TextureSystem&																   a_textureSystem);

		bool TransitionMaterialTexture(const DirectCommandList& a_directCommandList, const TextureSystem& a_textureSystem, const Struct::ModelMesh& a_modelMesh) const;

		bool DispatchModelMesh(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh) const;

	private:

		DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData> m_forwardPerObjectDataList = {};

		FWK_DEFINE_TYPE_INFO(StaticModelStandardPerObjectDrawRequestBase, DrawRequestPerObjectBase)
	};
}