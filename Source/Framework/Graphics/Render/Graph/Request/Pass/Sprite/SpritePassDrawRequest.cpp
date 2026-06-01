#include "SpritePassDrawRequest.h"

bool FWK::Graphics::SpritePassDrawRequest::SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource)
{
	const bool l_isSuccess = SetupConstantBuffer<SpritePassConstantBufferUploader, Tag::RootParameterCBSpritePassTag>(a_rootSignature,
																													  a_directCommandList,
																													  a_frameResource,
																													  GetREFConstantBuffer());

	FWK_ASSERT_RETURN_VALUE_IF(!l_isSuccess, "スプライト共通定数バッファの設定を行えませんでした。", false)

	return false;
}
