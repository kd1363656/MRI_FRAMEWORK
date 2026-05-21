#pragma once

namespace FWK::Graphics
{
	class DrawStaticModelLitStandardCommand final : public DrawRequestCommandBase<Struct::StaticModelLitStandardDrawCommand, Struct::StaticModelLitStandardPassConstant>
	{
	public:

		 DrawStaticModelLitStandardCommand()		  = default;
		~DrawStaticModelLitStandardCommand() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override;

	private:


		bool SetupCBModelObject(const RootSignature&							 a_rootSignature,
								const DirectCommandList&		                 a_directCommandList,
								const UploadBuffer&					             a_modelObjectUploadBuffer,
								const Struct::StaticModelLitStandardDrawCommand& a_staticModelLitStandardDrawCommand,
								const Struct::ModelMaterialRuntimeData&			 a_modelMaterialRuntimeData,
								const Struct::ModelMeshRuntimeData&			     a_modelMeshRuntimeData,
								const std::size_t&								 a_modelObjectIndex,
									  std::uint8_t* const			             a_modelObjectMappedData) const;

		bool ValidateModelMeshStructuredBufferSRV(const Struct::ModelMeshRuntimeData& a_modelMeshRuntimeData) const;
		
		FWK_DEFINE_TYPE_INFO(DrawStaticModelLitStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawStaticModelLitStandardCommand);