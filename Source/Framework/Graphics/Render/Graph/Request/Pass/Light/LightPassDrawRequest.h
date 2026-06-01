#pragma once

namespace FWK::Graphics
{
	class LightPassDrawRequest final : public CachedPassConstantDrawRequestBase<Struct::CBLightPass>
	{
	public:

		 LightPassDrawRequest()			 = default;
		~LightPassDrawRequest() override = default;

		bool SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource) override;

		FWK_DEFINE_TYPE_INFO(LightPassDrawRequest, DrawRequestPassBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryDrawRequestPass, FWK::Graphics::LightPassDrawRequest)