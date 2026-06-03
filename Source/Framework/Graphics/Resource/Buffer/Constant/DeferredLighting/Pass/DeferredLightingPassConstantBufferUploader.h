#pragma once

namespace FWK::Graphics
{
	class DeferredLightingPassConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 DeferredLightingPassConstantBufferUploader();
		~DeferredLightingPassConstantBufferUploader() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBDeferredLightingPass);

		FWK_DEFINE_TYPE_INFO(DeferredLightingPassConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryConstantBufferUploader, FWK::Graphics::DeferredLightingPassConstantBufferUploader)