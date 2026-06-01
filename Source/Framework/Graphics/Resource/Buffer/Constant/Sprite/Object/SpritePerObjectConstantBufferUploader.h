#pragma once

namespace FWK::Graphics
{
	class SpritePerObjectConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 SpritePerObjectConstantBufferUploader();
		~SpritePerObjectConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBSpritePerObject);

		FWK_DEFINE_TYPE_INFO(SpritePerObjectConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::SpritePerObjectConstantBufferUploader)