#pragma once

namespace FWK::Graphics
{
	class DrawStaticModelUnLitStandardCommand final : public DrawCommandBase<Struct::StaticModelUnLitStandardDrawCommand>
	{
	public:

		 DrawStaticModelUnLitStandardCommand()		    = default;
		~DrawStaticModelUnLitStandardCommand() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override;

	private:

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		// 現フレームで使用するCamera用定数バッファの先頭Index
		static constexpr std::size_t k_cbCameraIndex = 0ULL;

		bool SetupCBCamera(const std::weak_ptr<RootSignature>& a_rootSignature,
						   const Renderer&					  a_renderer,
						   const DirectCommandList&		      a_directCommandList,
						   const UploadBuffer&				  a_cameraUploadBuffer,
								 std::uint8_t* const		  a_cameraMappedData) const;

		bool SetupCBModelObject(const std::weak_ptr<RootSignature>&				   a_rootSignature,
								const Struct::StaticModelUnLitStandardDrawCommand& a_staticModelUnLitStandardDrawCommand,
								const DirectCommandList&		                   a_directCommandList,
								const UploadBuffer&					               a_cameraUploadBuffer,
								const std::size_t&								   a_modelObjectIndex,
									  std::uint8_t* const			               a_cameraMappedData) const;

		bool SetupModelMeshStructuredBufferSRV(const std::weak_ptr<RootSignature>&		a_rootSignature,
											   const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
											   const DirectCommandList&				    a_directCommandList,
											   const Struct::ModelMeshRuntimeData&	    a_modelMeshRuntimeData) const;

		FWK_DEFINE_TYPE_INFO(DrawStaticModelUnLitStandardCommand, IDrawCommand);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawStaticModelUnLitStandardCommand);