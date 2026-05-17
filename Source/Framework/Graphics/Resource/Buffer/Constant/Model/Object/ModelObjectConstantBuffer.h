#pragma once

namespace FWK::Graphics
{
	class ModelObjectConstantBuffer final : public ConstantBufferBase
	{
	public:

		 ModelObjectConstantBuffer();
		~ModelObjectConstantBuffer() override;

	private:

		static constexpr UINT64 k_bufferTypeSize = sizeof(Struct::CBModelObject);

		FWK_DEFINE_TYPE_INFO(ModelObjectConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::ModelObjectConstantBuffer);