#include "DrawSpriteStandardCommand.h"

void FWK::Graphics::DrawSpriteStandardCommand::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature<Tag::SpriteStandardPipelineStateTag>(a_renderer);
}

void FWK::Graphics::DrawSpriteStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer, TextureSystem& a_textureSystem)
{

	// スプライト用ルートシグネチャとパイプラインステートをセット
	// その際にセットしたルートシグネチャとパイプラインステートのポインタを取得
	SetupGraphicsPipelineStateToCommandList(a_renderer);

	const auto& l_rootSignature = GetVALRootSignature();

	if (l_rootSignature.expired())
	{
		assert(false && "使用しようとしたルートシグネチャが無効なため、描画処理に失敗しました。");
		return;
	}

	const auto& l_currentFrameResource = a_renderer.FetchVALCurrentFrameResource().lock();

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

	auto* const l_spriteDrawMappedData = l_spriteDrawUploadBuffer.Map();
	auto* const l_spritePassMappedData = l_spritePassUploadBuffer.Map();

	if (!l_spritePassMappedData)
	{
		assert(false && "スプライトパス用定数バッファのMapに失敗したため、描画処理に失敗しました。");
		return;
	}

	if (!l_spriteDrawMappedData)
	{
		assert(false && "スプライト描画用定数バッファのMapに失敗したため、描画処理に失敗しました。");
		l_spritePassUploadBuffer.UnMap();
		return;
	}

	// もし共通定数バッファの設定に失敗したらマップを解除
	if (!SetupCBSpritePass(l_rootSignature,
						   a_renderer,
						   l_directCommandList,
						   l_spritePassUploadBuffer,
						   l_spritePassMappedData))
	{
		l_spriteDrawUploadBuffer.UnMap();
		l_spritePassUploadBuffer.UnMap();
		return;
	}
	
	// 貯めこんでいたテクスチャ描画命令を回す
	const auto& l_spriteDrawCommandList = GetREFDrawCommandList();

	for (std::size_t l_spriteDrawCommandIndex = 0ULL; l_spriteDrawCommandIndex < l_spriteDrawCommandList.size(); ++l_spriteDrawCommandIndex)
	{
		const auto& l_spriteDrawCommand = l_spriteDrawCommandList[l_spriteDrawCommandIndex];
		
		const auto& l_textureRecord = l_spriteDrawCommand.m_textureRecord.lock();

		if (!l_textureRecord)					                          { continue; }
		if (!l_textureRecord->m_textureResource)                          { continue; }
		if (l_textureRecord->m_storageID == Constant::k_invalidStorageID) { continue; }

		// 現在のテクスチャの状態がD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEでなければそれにする
		TransitionTextureToPixelShaderResource(l_directCommandList, *l_textureRecord);

		// ディスクリプタテーブルにテクスチャをセット
		l_directCommandList.SetupDescriptorTable<Tag::RootParameterSpriteBaseColorTextureTag>(a_srvDescriptorPool.GetREFDescriptorHeap(), l_rootSignature, l_textureRecord->m_srvStorageID);

		if (!SetupCBSpriteDraw(l_rootSignature, 
							   l_spriteDrawCommand,
							   l_directCommandList,
							   l_spriteDrawUploadBuffer,
							   l_spriteDrawCommandIndex,
							   l_spriteDrawMappedData))
		{
			continue;
		}
		
		l_directCommandList.DispatchMesh(k_defaultDispatchMeshThreadGroupCountX, k_defaultDispatchMeshThreadGroupCountY, k_defaultDispatchMeshThreadGroupCountZ);
	}

	l_spriteDrawUploadBuffer.UnMap();
	l_spritePassUploadBuffer.UnMap();
}

bool FWK::Graphics::DrawSpriteStandardCommand::SetupCBSpritePass(const std::weak_ptr<RootSignature>& a_rootSignature,
																 const Renderer&					 a_renderer,
															     const DirectCommandList&			 a_directCommandList,
															     const UploadBuffer&				 a_spritePassUploadBuffer, 
																	   std::uint8_t* const			 a_spritePassMappedData) const
{
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

	return SetupConstantBuffer<Tag::RootParameterCBSpritePassTag>(a_rootSignature,
																  a_directCommandList,
																  a_spritePassUploadBuffer,
																  l_cbSpritePass,
																  k_cbSpritePassIndex,
																  a_spritePassMappedData);
}

bool FWK::Graphics::DrawSpriteStandardCommand::SetupCBSpriteDraw(const std::weak_ptr<RootSignature>& a_rootSignature,
																 const Struct::SpriteDrawCommand&    a_spriteDrawCommand, 
																 const DirectCommandList&            a_directCommandList,
																 const UploadBuffer&			     a_spriteDrawUploadBuffer,
																 const std::size_t				     a_spriteDrawCommandIndex, 
																	   std::uint8_t* const		     a_spriteDrawMappedData) const
{
	Struct::CBSpriteDraw l_cbSpriteDraw = {};

	l_cbSpriteDraw.m_color      = a_spriteDrawCommand.m_color;
	l_cbSpriteDraw.m_position   = a_spriteDrawCommand.m_position;
	l_cbSpriteDraw.m_scale      = a_spriteDrawCommand.m_scale;
	l_cbSpriteDraw.m_pivot      = a_spriteDrawCommand.m_pivot;
	l_cbSpriteDraw.m_sourceRECT = a_spriteDrawCommand.m_sourceRECT;

	return SetupConstantBuffer<Tag::RootParameterCBSpriteDrawTag>(a_rootSignature,
																  a_directCommandList,
																  a_spriteDrawUploadBuffer,
																  l_cbSpriteDraw,
																  a_spriteDrawCommandIndex,
																  a_spriteDrawMappedData);
}
