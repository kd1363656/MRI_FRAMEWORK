#pragma once

namespace FWK::Graphics
{
	class LightConstantBuffer final : public ConstantBufferBase
	{
	public:

		 LightConstantBuffer();
		~LightConstantBuffer() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBLight);

		FWK_DEFINE_TYPE_INFO(LightConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::LightConstantBuffer);