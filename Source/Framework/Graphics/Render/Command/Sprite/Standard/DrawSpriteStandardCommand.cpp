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

	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// PixelShaderからSRVを読むため、ShaderVisibleのSRVDescriptorHeapを設定する
	l_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	// 貯めこんでいたテクスチャ描画命令を回す
	const auto& l_spriteDrawCommandList = GetDrawCommandList();

	for (const auto& l_spriteDrawCommand : l_spriteDrawCommandList)
	{
		auto* l_textureRecord = a_textureSystem.FindMutablePTRTextureRecord(l_spriteDrawCommand.m_textureID);

		if (!l_textureRecord)					                                   { continue; }
		if (!l_textureRecord->m_textureResource)                                   { continue; }
		if (l_textureRecord->m_srvIndex == Constant::k_invalidDescriptorHeapIndex) { continue; }

		// 現在のテクスチャの状態がD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEでなければそれにする
		if (l_textureRecord->m_currentState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		{
			l_directCommandList.TransitionResource(l_textureRecord->m_textureResource,
												   l_textureRecord->m_currentState,
												   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			l_textureRecord->m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		}

		// ディスクリプタテーブルにテクスチャをセット
		l_directCommandList.SetupDescriptorTable<Tag::RootParameterSpriteBaseColorTextureTag>(a_srvDescriptorPool.GetREFDescriptorHeap(), l_graphicsPipelineStateSetupResult.m_rootSignature, l_textureRecord->m_srvIndex);

		l_directCommandList.DispatchMesh(k_defaultDispatchMeshThreadGroupCountX, k_defaultDispatchMeshThreadGroupCountY, k_defaultDispatchMeshThreadGroupCountZ);
	}
}