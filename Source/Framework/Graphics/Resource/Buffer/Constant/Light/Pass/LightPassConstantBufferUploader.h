#pragma once

namespace FWK::Graphics
{
	class LightPassConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 LightPassConstantBufferUploader();
		~LightPassConstantBufferUploader() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBLightPass);

		FWK_DEFINE_TYPE_INFO(LightPassConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryConstantBufferUploader, FWK::Graphics::LightPassConstantBufferUploader)