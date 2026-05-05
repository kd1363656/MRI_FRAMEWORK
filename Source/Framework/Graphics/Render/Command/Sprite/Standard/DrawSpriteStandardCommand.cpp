#include "DrawSpriteStandardCommand.h"

void FWK::Graphics::DrawSpriteStandardCommand::Draw(const Renderer& a_renderer, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, TextureSystem& a_textureSystem)
{
	// スプライト用ルートシグネチャとパイプラインステートをセット
	// その際にセットしたルートシグネチャとパイプラインステートのポインタを取得
	const auto& l_graphicsPipelineStateSetupResult = SetupGraphicsPipelineStateByTag<Tag::SpriteStandardPipelineStateTag>(a_renderer);

	if (!l_graphicsPipelineStateSetupResult.m_rootSignature)
	{
		assert(false && "使用しようとしたルートシグネチャが無効なため、描画処理に失敗しました。");
		return;
	}

	const auto* const l_currentFrameResource = a_renderer.FetchPTRCurrentFrameResource();

	if (!l_currentFrameResource)
	{
		assert(false && "現在のフレームリソースの取得に失敗しました。");
		return;
	}

	auto l_spritePassConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<SpritePassConstantBuffer>().lock();
	auto l_spriteDrawConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<SpriteDrawConstantBuffer>().lock();

	if (!l_spritePassConstantBuffer)
	{
		assert(false && "スプライトパス用定数バッファが取得できないため、描画処理に失敗しました。");
		return;
	}

	if (!l_spriteDrawConstantBuffer)
	{
		assert(false && "スプライト描画用定数バッファが取得できないため、描画処理に失敗しました。");
		return;
	}

	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// PixelShaderからSRVを読むため、ShaderVisibleのSRVDescriptorHeapを設定する
	l_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	const auto& l_spritePassUploadBuffer = l_spritePassConstantBuffer->GetREFUploadConstantBuffer();
	const auto& l_spriteDrawUploadBuffer = l_spriteDrawConstantBuffer->GetREFUploadConstantBuffer();

	auto* const l_spirteDrawMappedData = l_spriteDrawUploadBuffer.Map();
	auto* const l_spritePassMappedData = l_spritePassUploadBuffer.Map();

	if (!l_spritePassMappedData)
	{
		assert(false && "スプライトパス用定数バッファのMapに失敗したため、描画処理に失敗しました。");
		return;
	}

	if (!l_spirteDrawMappedData)
	{
		assert(false && "スプライト描画用定数バッファのMapに失敗したため、描画処理に失敗しました。");
		return;
	}

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

	// 正射影行列を格納ん
	l_cbSpritePass.m_projectionMatrix = l_projectionMatrix;

	// 格納した正射影行列をアップロードバッファにコピー
	std::memcpy(l_spritePassMappedData, &l_cbSpritePass, sizeof(Struct::CBSpritePass));

	// ルートシグネチャへスプライトパス共通定数バッファを結びつける
	l_directCommandList.SetupConstantBufferView<Tag::RootParameterCBSpritePassTag>(l_spritePassUploadBuffer.FetchVALGPUVirtualAddress(), l_graphicsPipelineStateSetupResult.m_rootSignature);

	const auto& l_cbSpriteDrawAlignedSize = Utility::Math::AlignUp(sizeof(Struct::CBSpriteDraw), Constant::k_constantBufferAlignment);

	// 貯めこんでいたテクスチャ描画命令を回す
	const auto& l_spriteDrawCommandList = GetREFDrawCommandList();

	for (std::size_t l_i = k_initialSpriteDrawCommandListIndex; l_i < l_spriteDrawCommandList.size(); ++l_i)
	{
		const auto l_spriteDrawCommand = l_spriteDrawCommandList[l_i];
		
		auto* l_textureRecord = a_textureSystem.FindMutablePTRTextureRecord(l_spriteDrawCommand.m_textureID);

		if (!l_textureRecord)					                                    { continue; }
		if (!l_textureRecord->m_textureResource)                                    { continue; }
		if ( l_textureRecord->m_srvIndex == Constant::k_invalidDescriptorHeapIndex) { continue; }

		// 現在のテクスチャの状態がD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEでなければそれにする
		if (l_textureRecord->m_currentState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		{
			l_directCommandList.TransitionResource(l_textureRecord->m_textureResource,
												   l_textureRecord->m_currentState,
												   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			l_textureRecord->m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		}

		Struct::CBSpriteDraw l_spriteDrawConstant = {};

		l_spriteDrawConstant.m_color      = l_spriteDrawCommand.m_color;
		l_spriteDrawConstant.m_position   = l_spriteDrawCommand.m_position;
		l_spriteDrawConstant.m_scale      = l_spriteDrawCommand.m_scale;
		l_spriteDrawConstant.m_pivot      = l_spriteDrawCommand.m_pivot;
		l_spriteDrawConstant.m_sourceRECT = l_spriteDrawCommand.m_sourceRECT;

		const auto& l_constantBufferOffset = l_i * l_cbSpriteDrawAlignedSize;

		std::memcpy(l_spirteDrawMappedData + l_constantBufferOffset, &l_spriteDrawConstant, sizeof(Struct::CBSpriteDraw));

		const auto l_gpuVirtualAddress = l_spriteDrawUploadBuffer.FetchVALGPUVirtualAddress() + l_constantBufferOffset;

		// ルートシグネチャへスプライト一枚分の定数バッファを結び付ける
		l_directCommandList.SetupConstantBufferView<Tag::RootParameterCBSpriteDrawTag>(l_gpuVirtualAddress, l_graphicsPipelineStateSetupResult.m_rootSignature);

		// ディスクリプタテーブルにテクスチャをセット
		l_directCommandList.SetupDescriptorTable<Tag::RootParameterSpriteBaseColorTextureTag>(a_srvDescriptorPool.GetREFDescriptorHeap(), l_graphicsPipelineStateSetupResult.m_rootSignature, l_textureRecord->m_srvIndex);

		l_directCommandList.DispatchMesh(k_defaultDispatchMeshThreadGroupCountX, k_defaultDispatchMeshThreadGroupCountY, k_defaultDispatchMeshThreadGroupCountZ);
	}

	l_spriteDrawUploadBuffer.UnMap();
	l_spritePassUploadBuffer.UnMap();
}