#include "DrawSpriteStandardCommand.h"

void FWK::Graphics::DrawSpriteStandardCommand::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetVALTag<Tag::SpriteStandardPipelineStateTag>());
}

void FWK::Graphics::DrawSpriteStandardCommand::Draw(Renderer& a_renderer)
{
	const auto& l_rootSignature = GetVALRootSignature().lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "使用しようとしたルートシグネチャが無効なため、描画処理に失敗しました。")

	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "現在のフレームリソースの取得に失敗しました。")

	const auto& l_viewport = a_renderer.GetREFRenderArea().GetREFViewport();

	// CreateOrthographic(描画空間の横幅、
	//					  描画空間の縦幅、
	//					  近クリップ面、
	//					  遠クリップ面);
	// Xは -Width  / 2 ~ Width  / 2
	// Yは -Height / 2 ~ Height / 2の範囲を画面へ投影する
	const auto l_projectionMatrix = TypeAlias::Math::Matrix::CreateOrthographic(l_viewport.Width,
																				l_viewport.Height,
																				k_defaultNearClip,
																				k_defaultFarClip);

	Struct::CBSpritePass l_cbSpritePass = {};

	// 正射影行列を格納
	l_cbSpritePass.m_projectionMatrix = l_projectionMatrix;

	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// もし共通定数バッファの設定に失敗したらマップを解除
	const bool l_isSuccess = !SetupCommonPassConstantBuffer<SpritePassConstantBufferUploader, Tag::RootParameterCBSpritePassTag>(*l_rootSignature,
																																 l_directCommandList,
																																 *l_currentFrameResource,
																																 l_cbSpritePass);

	FWK_ASSERT_RETURN_IF(l_isSuccess, "共通パスの定数バッファが設定できず、描画処理に失敗しました。")


	auto l_spriteObjectConstantBufferUploader = l_currentFrameResource->FindPTRConstantBufferUploader<SpriteObjectConstantBufferUploader>().lock();

	FWK_ASSERT_RETURN_IF(!l_spriteObjectConstantBufferUploader, "スプライト描画用定数バッファが取得できないため、描画処理に失敗しました。")

	// 貯めこんでいたテクスチャ描画命令を回す
	const auto& l_spriteStandardDrawCommandList = GetREFDrawCommandList();

	for (auto l_spriteDrawCommandIndex = 0ULL; l_spriteDrawCommandIndex < l_spriteStandardDrawCommandList.size(); ++l_spriteDrawCommandIndex)
	{
		const auto& l_spriteStandardDrawCommand = l_spriteStandardDrawCommandList[l_spriteDrawCommandIndex].lock();
		
		// テクスチャ描画コマンドが作成されていなければreturn
		if (!l_spriteStandardDrawCommand) { continue; }

		const auto& l_textureRecord = l_spriteStandardDrawCommand->m_textureRecord.lock();

		if (!l_textureRecord)								  { continue; }
		if (!l_textureRecord->GetREFGPUResource().m_resource) { continue; }
		
		// 現在のテクスチャの状態がD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEでなければそれにする
		TransitionTextureToPixelShaderResource(l_directCommandList, *l_textureRecord);

		Struct::CBSpriteObject l_cbSpriteObject = {};

		l_cbSpriteObject.m_color                    = l_spriteStandardDrawCommand->m_color;
		l_cbSpriteObject.m_position                 = l_spriteStandardDrawCommand->m_position;
		l_cbSpriteObject.m_scale                    = l_spriteStandardDrawCommand->m_scale;
		l_cbSpriteObject.m_pivot                    = l_spriteStandardDrawCommand->m_pivot;
		l_cbSpriteObject.m_sourceRECT               = l_spriteStandardDrawCommand->m_sourceRECT;
		l_cbSpriteObject.m_baseColorTextureSRVIndex = l_textureRecord->GetVALSRVStorageID();

		SetupPerDrawConstantBuffer<Tag::RootParameterCBSpriteObjectTag>(*l_rootSignature,
																		l_directCommandList,
																		l_cbSpriteObject,
																		*l_spriteObjectConstantBufferUploader);

		l_directCommandList.DispatchMesh(GetVALDefaultDispatchMeshThreadGroupCountX(), GetVALDefaultDispatchMeshThreadGroupCountY(), GetVALDefaultDispatchMeshThreadGroupCountZ());
	}
}