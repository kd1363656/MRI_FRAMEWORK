#pragma once

namespace FWK::Graphics
{
	class SpriteDrawConstantBuffer final : public ConstantBufferBase
	{
	public:

		 SpriteDrawConstantBuffer();
		~SpriteDrawConstantBuffer() override ;

		FWK_DEFINE_TYPE_INFO(SpriteDrawConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::SpriteDrawConstantBuffer);