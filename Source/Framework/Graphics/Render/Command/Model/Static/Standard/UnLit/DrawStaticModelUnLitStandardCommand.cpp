#include "DrawStaticModelUnLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelUnLitStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer)
{
	// 描画用定数バッファのセットを行う
	if (!DrawStaticModelStandardCommandBase::SetupCommonPassConstantBuffer(a_srvDescriptorPool, a_renderer))
	{
		assert(false && "共通定数であるカメラバッファがセットできておらず、StaticModel描画処理に失敗しました。");
		return;
	}

	const auto& l_rootSignature = GetVALRootSignature().lock();

	if (!l_rootSignature)
	{
		assert(false && "使用しようとしたルートシグネチャが無効なため、StaticModel描画処理に失敗しました。");
		return;
	}

	const auto& l_currentFrameResource = a_renderer.FetchVALCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "現在のフレームリソースの取得に失敗しました。");
		return;
	}

	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// モデルを一つ一つ描画していく
	DrawStaticModelStandardCommandBase::SetupCBModelObject(*l_rootSignature, l_directCommandList, *l_currentFrameResource);
}