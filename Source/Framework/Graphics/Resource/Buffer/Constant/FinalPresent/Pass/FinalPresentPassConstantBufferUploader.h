#pragma once

namespace FWK::Graphics
{
	class FinalPresentPassConstantBufferUploader final : public ConstantBufferUploaderBase
	{
	public:

		 FinalPresentPassConstantBufferUploader();
		~FinalPresentPassConstantBufferUploader() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBFinalPresent);

		FWK_DEFINE_TYPE_INFO(FinalPresentPassConstantBufferUploader, ConstantBufferUploaderBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBufferUploader, FWK::Graphics::FinalPresentPassConstantBufferUploader)