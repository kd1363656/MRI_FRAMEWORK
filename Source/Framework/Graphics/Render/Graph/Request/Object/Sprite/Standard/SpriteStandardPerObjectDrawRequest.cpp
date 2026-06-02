#include "SpriteStandardPerObjectDrawRequest.h"

void FWK::Graphics::SpriteStandardPerObjectDrawRequest::BeginFrame()
{
	// 前フレームのSprite描画申請を消す
	m_drawRequestPerObjectList.BeginFrame();
}

void FWK::Graphics::SpriteStandardPerObjectDrawRequest::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetVALTag<Tag::SpriteStandardPipelineStateTag>());
}

void FWK::Graphics::SpriteStandardPerObjectDrawRequest::RequestDraw(Renderer& a_renderer)
{
	const auto& l_rootSignature = GetVALRootSignature().lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "使用しようとしたルートシグネチャが無効なため、描画処理に失敗しました。")

	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "現在のフレームリソースの取得に失敗しました。")


}

void FWK::Graphics::SpriteStandardPerObjectDrawRequest::AddDrawRequestPerObject(const std::shared_ptr<Struct::SpriteStandardPerObjectDrawRequestData>& a_drawRequestData)
{
	m_drawRequestPerObjectList.AddDrawRequestPerObject(a_drawRequestData);
}