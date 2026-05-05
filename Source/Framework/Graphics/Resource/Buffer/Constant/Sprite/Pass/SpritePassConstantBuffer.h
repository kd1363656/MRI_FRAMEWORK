#pragma once

namespace FWK::Graphics
{
	class SpritePassConstantBuffer final : public ConstantBufferBase
	{
	public:

		 SpritePassConstantBuffer();
		~SpritePassConstantBuffer() override ;

		FWK_DEFINE_TYPE_INFO(SpritePassConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::SpritePassConstantBuffer);