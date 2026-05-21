#pragma once

namespace FWK::Graphics
{
	class DrawStaticModelUnLitStandardCommand final : public DrawRequestCommandBase<Struct::StaticModelUnLitStandardDrawCommand, Struct::StaticModelUnLitStandardPassConstant>
	{
	public:

		 DrawStaticModelUnLitStandardCommand()		    = default;
		~DrawStaticModelUnLitStandardCommand() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override;

	private:

		bool SetupCBModelObject(const RootSignature&							   a_rootSignature,
								const DirectCommandList&		                   a_directCommandList,
								const UploadBuffer&					               a_modelObjectUploadBuffer,
								const Struct::StaticModelUnLitStandardDrawCommand& a_staticModelUnLitStandardDrawCommand,
								const Struct::ModelMaterialRuntimeData&			   a_modelMaterialRuntimeData,
								const Struct::ModelMeshRuntimeData&			       a_modelMeshRuntimeData,
								const std::size_t&								   a_modelObjectIndex,
									  std::uint8_t* const			               a_modelObjectMappedData) const;

		bool ValidateModelMeshStructuredBufferSRV(const Struct::ModelMeshRuntimeData& a_modelMeshRuntimeData) const;
		
		FWK_DEFINE_TYPE_INFO(DrawStaticModelUnLitStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawStaticModelUnLitStandardCommand);