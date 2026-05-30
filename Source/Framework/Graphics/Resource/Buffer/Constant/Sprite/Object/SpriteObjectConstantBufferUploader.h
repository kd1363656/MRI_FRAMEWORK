#pragma once

namespace FWK::Graphics
{
	class SpriteObjectConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 SpriteObjectConstantBufferUploader();
		~SpriteObjectConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBSpriteObject);

		FWK_DEFINE_TYPE_INFO(SpriteObjectConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::SpriteObjectConstantBufferUploader)