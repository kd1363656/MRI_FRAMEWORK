#pragma once

namespace FWK::Graphics
{
	class FinalPresentConstantBuffer final : public ConstantBufferBase
	{
	public:

		 FinalPresentConstantBuffer();
		~FinalPresentConstantBuffer() override ;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBFinalPresent);

		FWK_DEFINE_TYPE_INFO(FinalPresentConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::FinalPresentConstantBuffer);