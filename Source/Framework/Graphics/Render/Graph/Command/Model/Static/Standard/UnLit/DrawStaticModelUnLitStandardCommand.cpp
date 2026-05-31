#include "DrawStaticModelUnLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelUnLitStandardCommand::Draw(Renderer& a_renderer)
{
	// 描画用定数バッファのセットを行う
	if (!DrawStaticModelStandardCommandBase::SetupCommonPassConstantBuffer(a_renderer)) { return; }

	const auto& l_rootSignature = GetVALRootSignature().lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "使用しようとしたルートシグネチャが無効なため、StaticModel描画処理に失敗しました。")

	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "現在のフレームリソースの取得に失敗しました。")
	
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// モデルを一つ一つ描画していく
	DrawStaticModelStandardCommandBase::SetupCBModelObject(*l_rootSignature, l_directCommandList, *l_currentFrameResource);
}