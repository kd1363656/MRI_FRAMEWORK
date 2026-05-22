#include "DrawSpriteStandardCommand.h"

void FWK::Graphics::DrawSpriteStandardCommand::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetTag<Tag::SpriteStandardPipelineStateTag>());
}

void FWK::Graphics::DrawSpriteStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer)
{
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// PixelShaderからSRVを読むため、ShaderVisibleのSRVDescriptorHeapを設定する
	l_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());


	// スプライト用ルートシグネチャとパイプラインステートをセット
	// その際にセットしたルートシグネチャとパイプラインステートのポインタを取得
	SetupGraphicsPipelineStateToCommandList(a_renderer);

	const auto& l_rootSignature = GetVALRootSignature().lock();

	if (!l_rootSignature)
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
	if (!SetupCBSpritePass(*l_rootSignature,
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

	for (auto l_spriteDrawCommandIndex = 0ULL; l_spriteDrawCommandIndex < l_spriteDrawCommandList.size(); ++l_spriteDrawCommandIndex)
	{
		const auto& l_spriteDrawCommand = l_spriteDrawCommandList[l_spriteDrawCommandIndex];
		
		const auto& l_textureRecord = l_spriteDrawCommand.m_textureRecord.lock();

		if (!l_textureRecord)					        { continue; }
		if (!l_textureRecord->m_gpuResource.m_resource) { continue; }
		
		// 現在のテクスチャの状態がD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEでなければそれにする
		TransitionTextureToPixelShaderResource(l_directCommandList, *l_textureRecord);

		if (!SetupCBSpriteDraw(*l_rootSignature, 
							   l_directCommandList,
							   l_spriteDrawUploadBuffer,
							   *l_textureRecord,
							   l_spriteDrawCommand,
							   l_spriteDrawCommandIndex,
							   l_spriteDrawMappedData))
		{
			continue;
		}
		
		l_directCommandList.DispatchMesh(GetVALDefaultDispatchMeshThreadGroupCountX(), GetVALDefaultDispatchMeshThreadGroupCountY(), GetVALDefaultDispatchMeshThreadGroupCountZ());
	}

	l_spriteDrawUploadBuffer.UnMap();
	l_spritePassUploadBuffer.UnMap();
}

bool FWK::Graphics::DrawSpriteStandardCommand::SetupCBSpritePass(const RootSignature&	   a_rootSignature,
																 const Renderer&		   a_renderer,
															     const DirectCommandList&  a_directCommandList,
															     const UploadBuffer&	   a_spritePassUploadBuffer, 
																	   std::uint8_t* const a_spritePassMappedData) const
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

bool FWK::Graphics::DrawSpriteStandardCommand::SetupCBSpriteDraw(const RootSignature&					  a_rootSignature,
																 const DirectCommandList&                 a_directCommandList,
																 const UploadBuffer&			          a_spriteDrawUploadBuffer,
																 const Struct::TextureRecord&			  a_textureRecord,
																 const Struct::SpriteStandardDrawCommand& a_spriteStandardDrawCommand,
																 const std::size_t&				          a_spriteDrawCommandIndex, 
																	   std::uint8_t* const		          a_spriteDrawMappedData) const
{
	Struct::CBSpriteObject l_cbSpriteObject = {};

	l_cbSpriteObject.m_color                 = a_spriteStandardDrawCommand.m_color;
	l_cbSpriteObject.m_position              = a_spriteStandardDrawCommand.m_position;
	l_cbSpriteObject.m_scale                 = a_spriteStandardDrawCommand.m_scale;
	l_cbSpriteObject.m_pivot                 = a_spriteStandardDrawCommand.m_pivot;
	l_cbSpriteObject.m_sourceRECT            = a_spriteStandardDrawCommand.m_sourceRECT;
	l_cbSpriteObject.m_baseColorTextureIndex = a_textureRecord.m_srvStorageID;

	return SetupConstantBuffer<Tag::RootParameterCBSpriteObjectTag>(a_rootSignature,
																    a_directCommandList,
																    a_spriteDrawUploadBuffer,
																    l_cbSpriteObject,
																    a_spriteDrawCommandIndex,
																    a_spriteDrawMappedData);
}