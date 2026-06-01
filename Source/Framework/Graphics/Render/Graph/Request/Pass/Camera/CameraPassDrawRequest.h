#pragma once

namespace FWK::Graphics
{
	class CameraPassDrawRequest final : public CachedPassConstantDrawRequestBase<Struct::CBCameraPass>
	{
	public:

		 CameraPassDrawRequest()		  = default;
		~CameraPassDrawRequest() override = default;

		bool SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource) override;

		FWK_DEFINE_TYPE_INFO(CameraPassDrawRequest, DrawRequestPassBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryDrawRequestPass, FWK::Graphics::CameraPassDrawRequest)