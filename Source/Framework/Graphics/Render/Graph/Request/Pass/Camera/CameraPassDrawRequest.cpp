#include "CameraPassDrawRequest.h"

bool FWK::Graphics::CameraPassDrawRequest::SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource)
{
	const bool l_isSuccess = SetupConstantBuffer<CameraPassConstantBufferUploader, Tag::RootParameterCBCameraPassTag>(a_rootSignature,
																													  a_directCommandList,
																													  a_frameResource,
																													  GetREFConstantBuffer());

	FWK_ASSERT_RETURN_VALUE_IF(!l_isSuccess, "カメラ定数バッファの設定を行えませんでした。", false)

	return true;
}