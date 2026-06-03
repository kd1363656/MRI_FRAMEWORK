#pragma once

namespace FWK::Graphics
{
	class StaticModelStandardPerObjectDrawRequestBase : public DrawRequestPerObjectBase
	{
	public:

		 StaticModelStandardPerObjectDrawRequestBase()			= default;
		~StaticModelStandardPerObjectDrawRequestBase() override = default;

		void BeginFrame() override;

		virtual void RequestForwardDraw (const TextureSystem& a_textureSystem, Renderer& a_renderer) = 0;
		virtual void RequestDeferredDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer) { /*必要に応じてオーバーライドしてください*/ };

		void AddDrawRequestRenderPath(const std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestData, const Enum::StaticModelRenderingPath a_staticModelRenderingPath);

	protected:

		void AddDrawRequestRenderPath(const std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestData, DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData>& a_addPerObjectDataList, DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData>& a_removePerObjectDataList) const;

		void SetupModelMeshConstantBuffer(const RootSignature&																   a_rootSignature,
										  const DirectCommandList&															   a_directCommandList,
										  const FrameResource&																   a_frameResource,
										  const DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestDataList,
										  const TextureSystem&																   a_textureSystem);

		bool TransitionMaterialTexture(const DirectCommandList& a_directCommandList, const TextureSystem& a_textureSystem, const Struct::ModelMesh& a_modelMesh) const;

		bool DispatchModelMesh(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh) const;

		const auto& GetREFForwardPerObjectDataList () const { return m_forwardPerObjectDataList; }
		const auto& GetREFDeferredPerObjectDataList() const { return m_deferredPerObjectDataList; }

	private:

		DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData> m_forwardPerObjectDataList  = {};
		DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData> m_deferredPerObjectDataList = {};

		FWK_DEFINE_TYPE_INFO(StaticModelStandardPerObjectDrawRequestBase, DrawRequestPerObjectBase)
	};
}