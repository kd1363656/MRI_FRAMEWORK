#include "DrawStaticModelLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelLitStandardCommand::Draw(Renderer& a_renderer)
{
	// 描画用定数バッファのセットを行う
	if (!DrawStaticModelStandardCommandBase::SetupCommonPassConstantBuffer(a_renderer))
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

	const auto& l_lightSystem		= a_renderer.GetREFLightSystem	    ();
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	auto l_lightSystemConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<LightConstantBuffer>().lock();

	if (!l_lightSystemConstantBuffer)
	{
		assert(false && "Light用定数バッファが取得できないため、StaticModelLit描画処理に失敗しました。");
		return;
	}

	const auto&		  l_lightSystemUploadBuffer = l_lightSystemConstantBuffer->GetREFUploadConstantBuffer();
		  auto* const l_lightSystemMappedData   = l_lightSystemUploadBuffer.Map							 ();

	if (!l_lightSystemMappedData)
	{
		assert(false && "Light用定数バッファが取得できないため、StaticModelLit描画処理に失敗しました。");
		return;
	}

	const auto& l_cbLight = l_lightSystem.CreateCBLight();

	if (!SetupConstantBuffer<FWK::Tag::RootParameterCBLightTag>(*l_rootSignature,
																l_directCommandList,
																l_lightSystemUploadBuffer,
																l_cbLight,
																GetREFCommonPassIndex(),
																l_lightSystemMappedData))
	{
		assert(false && "Light用定数バッファが設定できておらず、StaticModelLit描画処理に失敗しました。");
		return;
	}

	// モデルを一つ一つ描画していく
	DrawStaticModelStandardCommandBase::SetupCBModelObject(*l_rootSignature, l_directCommandList, *l_currentFrameResource);
}