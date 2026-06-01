#pragma once

namespace FWK::Graphics
{
	class ModelPerObjectConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 ModelPerObjectConstantBufferUploader();
		~ModelPerObjectConstantBufferUploader() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBModelPerObject);

		FWK_DEFINE_TYPE_INFO(ModelPerObjectConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::ModelPerObjectConstantBufferUploader)