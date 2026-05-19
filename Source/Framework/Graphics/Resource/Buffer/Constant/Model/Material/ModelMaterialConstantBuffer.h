#pragma once

namespace FWK::Graphics
{
	class ModelMaterialConstantBuffer final : public ConstantBufferBase
	{
	public:

		 ModelMaterialConstantBuffer();
		~ModelMaterialConstantBuffer() override;

	private:

		static constexpr UINT k_bufferTypeSize = sizeof(Struct::CBModelMaterial);

		FWK_DEFINE_TYPE_INFO(ModelMaterialConstantBuffer, ConstantBufferBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryConstantBuffer, FWK::Graphics::ModelMaterialConstantBuffer);