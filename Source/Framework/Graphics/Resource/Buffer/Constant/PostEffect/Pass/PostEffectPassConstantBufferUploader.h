#pragma once

namespace FWK::Graphics
{
	class PostEffectPassConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 PostEffectPassConstantBufferUploader();
		~PostEffectPassConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBPostEffectPass);

		FWK_DEFINE_TYPE_INFO(PostEffectPassConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::PostEffectPassConstantBufferUploader)