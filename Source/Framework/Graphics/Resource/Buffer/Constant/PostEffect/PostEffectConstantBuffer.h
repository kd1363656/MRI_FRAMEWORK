#pragma once

namespace FWK::Graphics
{
	class PostEffectConstantBuffer final : public ConstantBufferBase
	{
	public:

		 PostEffectConstantBuffer();
		~PostEffectConstantBuffer() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBPostEffect);

		FWK_DEFINE_TYPE_INFO(PostEffectConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::PostEffectConstantBuffer);