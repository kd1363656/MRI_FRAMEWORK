#pragma once

namespace FWK::Graphics
{
	class PostEffectConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 PostEffectConstantBufferUploader();
		~PostEffectConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBPostEffect);

		FWK_DEFINE_TYPE_INFO(PostEffectConstantBufferUploader, ConstantBufferUploaderBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::PostEffectConstantBufferUploader);