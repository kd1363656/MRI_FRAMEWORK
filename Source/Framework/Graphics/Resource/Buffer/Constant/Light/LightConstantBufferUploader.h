#pragma once

namespace FWK::Graphics
{
	class LightConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 LightConstantBufferUploader();
		~LightConstantBufferUploader() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBLight);

		FWK_DEFINE_TYPE_INFO(LightConstantBufferUploader, ConstantBufferUploaderBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::LightConstantBufferUploader)