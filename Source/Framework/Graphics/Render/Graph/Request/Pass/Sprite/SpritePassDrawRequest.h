#pragma once

namespace FWK::Graphics
{
	class SpritePassDrawRequest final : public CachedPassConstantDrawRequestBase<Struct::CBSpritePass>
	{
	public:

		 SpritePassDrawRequest()		  = default;
		~SpritePassDrawRequest() override = default;

		bool SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource) override;

		FWK_DEFINE_TYPE_INFO(SpritePassDrawRequest, DrawRequestPassBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryDrawRequestPass, FWK::Graphics::SpritePassDrawRequest)