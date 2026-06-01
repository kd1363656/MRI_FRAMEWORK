#include "LightPassDrawRequest.h"

bool FWK::Graphics::LightPassDrawRequest::SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource)
{
	const bool l_isSuccess = SetupConstantBuffer<LightPassConstantBufferUploader, Tag::RootParameterCBLightPassTag>(a_rootSignature,
																													a_directCommandList,
																													a_frameResource,
																													GetREFConstantBuffer());

	FWK_ASSERT_RETURN_VALUE_IF(!l_isSuccess, "ライト定数バッファの設定を行えませんでした。", false)

	return true;
}