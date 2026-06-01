#pragma once

namespace FWK::Graphics
{
	class SpritePassConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 SpritePassConstantBufferUploader();
		~SpritePassConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBSpritePass);

		FWK_DEFINE_TYPE_INFO(SpritePassConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryConstantBufferUploader, FWK::Graphics::SpritePassConstantBufferUploader)