#pragma once

namespace FWK::Graphics
{
	class ModelObjectConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 ModelObjectConstantBufferUploader();
		~ModelObjectConstantBufferUploader() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBModelObject);

		FWK_DEFINE_TYPE_INFO(ModelObjectConstantBufferUploader, ConstantBufferUploaderBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::ModelObjectConstantBufferUploader);