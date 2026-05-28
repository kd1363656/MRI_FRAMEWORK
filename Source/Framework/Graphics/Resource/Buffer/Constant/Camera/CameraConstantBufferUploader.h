#pragma once

namespace FWK::Graphics
{
	class CameraConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 CameraConstantBufferUploader();
		~CameraConstantBufferUploader() override;

	private:

		static constexpr UINT64  k_bufferTypeSize = sizeof(Struct::CBCamera);

		FWK_DEFINE_TYPE_INFO(CameraConstantBufferUploader, ConstantBufferUploaderBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::CameraConstantBufferUploader);