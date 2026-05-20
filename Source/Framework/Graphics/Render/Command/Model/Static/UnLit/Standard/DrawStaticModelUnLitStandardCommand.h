#pragma once

namespace FWK::Graphics
{
	class DrawStaticModelUnLitStandardCommand final : public DrawRequestCommandBase<Struct::StaticModelUnLitStandardDrawCommand>
	{
	public:

		 DrawStaticModelUnLitStandardCommand()		    = default;
		~DrawStaticModelUnLitStandardCommand() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override;

	private:

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		bool SetupCBModelObject(const std::weak_ptr<RootSignature>&				   a_rootSignature,
								const Struct::StaticModelUnLitStandardDrawCommand& a_staticModelUnLitStandardDrawCommand,
								const Struct::ModelMaterialRuntimeData&			   a_modelMaterialRuntimeData,
								const DirectCommandList&		                   a_directCommandList,
								const UploadBuffer&					               a_modelObjectUploadBuffer,
								const std::size_t&								   a_modelObjectIndex,
									  std::uint8_t* const			               a_modelObjectMappedData) const;

		bool SetupModelMeshStructuredBufferSRV(const std::weak_ptr<RootSignature>&		a_rootSignature,
											   const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
											   const DirectCommandList&				    a_directCommandList,
											   const Struct::ModelMeshRuntimeData&	    a_modelMeshRuntimeData) const;

		FWK_DEFINE_TYPE_INFO(DrawStaticModelUnLitStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawStaticModelUnLitStandardCommand);