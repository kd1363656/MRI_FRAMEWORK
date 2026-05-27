#pragma once

namespace FWK::Graphics
{
	class SpriteObjectConstantBuffer final : public ConstantBufferBase
	{
	public:

		 SpriteObjectConstantBuffer();
		~SpriteObjectConstantBuffer() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBSpriteObject);

		FWK_DEFINE_TYPE_INFO(SpriteObjectConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::SpriteObjectConstantBuffer);