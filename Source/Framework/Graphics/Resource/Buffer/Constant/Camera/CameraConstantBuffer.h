#pragma once

namespace FWK::Graphics
{
	class CameraConstantBuffer final : public ConstantBufferBase
	{
	public:

		 CameraConstantBuffer();
		~CameraConstantBuffer() override;

	private:

		static constexpr UINT64  k_bufferTypeSize = sizeof(Struct::CBCamera);

		FWK_DEFINE_TYPE_INFO(CameraConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::CameraConstantBuffer);