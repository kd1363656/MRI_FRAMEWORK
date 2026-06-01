#pragma once

namespace FWK::Graphics
{
	class CameraPassConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 CameraPassConstantBufferUploader();
		~CameraPassConstantBufferUploader() override;

	private:

		static constexpr UINT64  k_bufferTypeSize = sizeof(Struct::CBCameraPass);

		FWK_DEFINE_TYPE_INFO(CameraPassConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryConstantBufferUploader, FWK::Graphics::CameraPassConstantBufferUploader)