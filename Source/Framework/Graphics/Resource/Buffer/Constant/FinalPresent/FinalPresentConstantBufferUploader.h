#pragma once

namespace FWK::Graphics
{
	class FinalPresentConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 FinalPresentConstantBufferUploader();
		~FinalPresentConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBFinalPresent);

		FWK_DEFINE_TYPE_INFO(FinalPresentConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::FinalPresentConstantBufferUploader)