#pragma once

namespace FWK::Graphics
{
	class SpritePassConstantBuffer final : public ConstantBufferBase
	{
	public:

		 SpritePassConstantBuffer();
		~SpritePassConstantBuffer() override ;

		FWK_DEFINE_TYPE_INFO(SpritePassConstantBuffer, ConstantBufferBase);

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBSpritePass);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::SpritePassConstantBuffer);