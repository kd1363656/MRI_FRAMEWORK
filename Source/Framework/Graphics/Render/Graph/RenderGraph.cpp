#include "RenderGraph.h"

void FWK::Graphics::RenderGraph::INIT()
{
	m_passList.clear		   ();
	m_sortedPassIndexList.clear();
}

bool FWK::Graphics::RenderGraph::Compile()
{
	m_sortedPassIndexList.clear();

	const auto l_passCount = m_passList.size();

	if (m_passList.empty())
	{
		assert(false && "RenderGraphPassが登録されていないため、Compileに失敗しました。");
		return false;
	}

	std::vector<std::vector<std::uint32_t>> l_edgeList	   = {};
	std::vector<std::uint32_t>			    l_inDegreeList = {};

	l_edgeList.resize	 (l_passCount);
	l_inDegreeList.resize(l_passCount);

	// 各PassのRead/Write情報を見て、Pass同士の依存関係を作成する
	// 例 : 
	// GBufferPass  : GBufferNormalへWrite
	// LightingPass : GBufferNormalをRead
	// この場合、GBufferPassが終わるまでLightingPassは実行できないため、
	// GBufferPass -> LightingPassという依存関係を作る
	BuildDependency(l_edgeList, l_inDegreeList);

	std::queue<std::uint32_t> l_passQueue = {};

	for (std::uint32_t l_passIndex = 0U; l_passIndex < l_passCount; ++l_passIndex)
	{
		// 入次数は「このPassより前に終わっていないといけないPassの数」
		// 0の場合は、他のPassを待たずに実行できる
		if (l_inDegreeList[l_passIndex] == k_noRenderGraphIncomingEdgeCount)
		{
			l_passQueue.emplace(l_passIndex);
		}
	}

	while (!l_passQueue.empty())
	{
		// 現時点で実行可能なPassを一つ取り出す
		const auto l_passIndex = l_passQueue.front();

		l_passQueue.pop();

		// 実行順リストへ追加する
		// このm_sortedPassIndexListが、最終的なRenderGraphの実行順になる
		m_sortedPassIndexList.emplace_back(l_passIndex);

		// このPassが終わることで、次のPassが待っている依存数を一つ減らせる
		for (const auto l_nextPassIndex : l_edgeList[l_passIndex])
		{
			--l_inDegreeList[l_nextPassIndex];

			// 依存数が0になったPassは、実行可能になったのでキューへ追加する
			if (l_inDegreeList[l_nextPassIndex] == k_noRenderGraphIncomingEdgeCount)
			{
				l_passQueue.emplace(l_nextPassIndex);
			}
		}
	}

	// 全Passを並べられなかった場合、依存関係が循環している
	// 例 : 
	// AはBの後に実行したい、
	// BはAの後に実行したい、
	// このような状態では正しい実行順序を作れない
	if (m_sortedPassIndexList.size() != l_passCount)
	{
		assert(false && "RenderGraphPassの依存関係が循環しているため、Compileに失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::RenderGraph::Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap, 
										 const DSVDescriptorHeap&				  a_dsvDescriptorHeap, 
										 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
										 const SwapChain&						  a_swapChain, 
											   DirectCommandList&				  a_directCommandList,
											   Renderer&						  a_renderer)
{
	if (m_sortedPassIndexList.empty())
	{
		assert(false && "RenderGraphがCompileされていないため、実行できませんでした。");
		return;
	}

	for (const auto l_passIndex : m_sortedPassIndexList)
	{
		if (m_passList.size() <= l_passIndex)
		{
			assert(false && "RenderGraphPassIndexが範囲外です。");
			return;
		}

		const auto& l_pass = m_passList[l_passIndex];

		if (!l_pass)
		{
			assert(false && "RenderGraphPassが無効です。");
			return;
		}

		// Passが必要としているResourceStateへ自動で遷移させる
		// 例 : 
		// SceneDrawPass   : SceneColorをRENDER_TARGETとしてWrite
		// PresentCopyPass : SceneColorをCOPY_SOURCEとしてRead
		// この場合、PresentCopyPass実行前に
		// RENDER_TARGET -> COPY_SOURCEのResourceBarrierを張る
		TransitionPassTexture(*l_pass, a_directCommandList, a_renderer);

		l_pass->Execute(a_rtvDescriptorHeap,
						a_dsvDescriptorHeap,
						a_srvDescriptorPool,
						a_swapChain,	
						a_directCommandList,
						a_renderer);
	}
}

bool FWK::Graphics::RenderGraph::IsReadAccess(const Struct::RenderGraphTextureAccess& a_textureAccess) const
{
	return a_textureAccess.m_accessTag == Utility::Tag::GetTag<Tag::RenderGraphReadAccessTag>();
}
bool FWK::Graphics::RenderGraph::IsWriteAccess(const Struct::RenderGraphTextureAccess& a_textureAccess) const
{
	return a_textureAccess.m_accessTag == Utility::Tag::GetTag<Tag::RenderGraphWriteAccessTag>();
}

void FWK::Graphics::RenderGraph::AddDependencyEdge(const std::uint32_t							  a_fromPassIndex, 
												   const std::uint32_t							  a_toPassIndex, 
														 std::vector<std::vector<std::uint32_t>>& a_edgeList,
														 std::vector<std::uint32_t>&		      a_inDegreeList) const
{
	if (a_fromPassIndex == a_toPassIndex) { return; }

	for (const auto l_nextPassIndex : a_edgeList[a_fromPassIndex])
	{
		if (l_nextPassIndex == a_toPassIndex) { return; }
	}

	// a_fromPassIndexのPassが終わった後に、a_toPassIndexのPassを実行できるという意味
	a_edgeList[a_fromPassIndex].emplace_back(a_toPassIndex);

	// a_toPassIndex側は、待たないといけないPassが一つ増える
	++a_inDegreeList[a_toPassIndex];
}

void FWK::Graphics::RenderGraph::BuildDependency(std::vector<std::vector<std::uint32_t>>& a_edgeList, std::vector<std::uint32_t>& a_inDegreeList) const
{
	const auto l_passCount = m_passList.size();

	for (std::uint32_t l_prevPassIndex = 0U; l_prevPassIndex < l_passCount; ++l_prevPassIndex)
	{
		for (std::uint32_t l_nextPassIndex = l_prevPassIndex + k_nextRenderGraphPassIndexOffset; l_nextPassIndex < l_passCount; ++l_nextPassIndex)
		{
			const auto& l_prevPass = m_passList[l_prevPassIndex];
			const auto& l_nextPass = m_passList[l_nextPassIndex];

			if (!l_prevPass) 
			{
				assert(false && "前回のRenderGraphPassが無効です。");
				return;
			}

			if (!l_nextPass)
			{
				assert(false && "次回のRenderGraphPassが無効です。");
				return;
			}

			for (const auto& l_prevTextureAccess : l_prevPass->GetREFTextureAccessList())
			{
				for (const auto& l_nextTextureAccess : l_nextPass->GetREFTextureAccessList())
				{
					// 違うTextureTagなら、同じリソースを使っていないので循環関係は作らない
					if (l_prevTextureAccess.m_textureTag != l_nextTextureAccess.m_textureTag) { continue; }

					const auto l_isPrevRead  = IsReadAccess (l_prevTextureAccess);
					const auto l_isPrevWrite = IsWriteAccess(l_prevTextureAccess);
					const auto l_isNextRead  = IsReadAccess (l_nextTextureAccess);
					const auto l_isNextWrite = IsWriteAccess(l_nextTextureAccess);
					
					if ((!l_isPrevRead && !l_isPrevWrite) || (!l_isNextRead && !l_isNextWrite))
					{
						assert(false && "RenderGraphTextureAccessのAccessTagが不正です。");
						return;
					}

					// Read -> Read
					// 両方とも読むだけなので、どちらが先でもリソース内容は変わらない
					// そのため依存関係は作らない
					if (l_isPrevRead && l_isNextRead) { continue; }

					// Write -> Read
					// 前のPassが書き込んだ結果を、次のPassが読む
					// 例 : 
					// GBufferPass  : NormalTextureへWrite
					// LightingPass : NormalTextureをRead
					// この場合、GBufferPass -> LightingPassの順番が必要
					if (l_isPrevWrite && l_isNextRead)
					{
						AddDependencyEdge(l_prevPassIndex,
										  l_nextPassIndex,
										  a_edgeList,
										  a_inDegreeList);

						continue;
					}

					// Read -> Write
					// 前のPassが読んでいる間に、次のPassが同じリソースを置き換えると危険
					// そのため登録順序を守り、前のPass -> 次のPassの順番にする
					// 注意 : 
					// ここで勝手にWriteを前へ移動すると、前フレーム履歴テクスチャなどで意図しない順番になる可能性がある
					if (l_isPrevRead && l_isNextWrite)
					{
						AddDependencyEdge(l_prevPassIndex,
										  l_nextPassIndex,
										  a_edgeList,
										  a_inDegreeList);

						continue;
					}
					
					// Write -> Write
					// 同じリソースへ複数Passが書き込む場合、登録順を守る
					// 例 : 
					// PassA : SceneColorへWrite
					// PassB : SceneColorへWrite
					// この場合、どちらの結果を最終結果にするかが順番で決まる
					if (l_isPrevWrite && l_isNextWrite)
					{
						AddDependencyEdge(l_prevPassIndex,
										  l_nextPassIndex,
										  a_edgeList,
										  a_inDegreeList);

						continue;
					}
				}
			}
		}
	}
}

void FWK::Graphics::RenderGraph::TransitionPassTexture(const IRenderGraphPass& a_pass, DirectCommandList& a_directCommandList, Renderer& a_renderer)
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "FrameResourceが無効のため、RenderGraphPass用Textureの状態遷移ができませんでした。");
		return;
	}

	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();

	for (const auto& l_textureAccess : a_pass.GetREFTextureAccessList())
	{
		if (TransitionRenderTargetTexture(l_textureAccess, l_renderGraphResourceRegistry, a_directCommandList)) { continue; }
		if (TransitionDepthStencilTexture(l_textureAccess, l_renderGraphResourceRegistry, a_directCommandList)) { continue; }

		assert(false && "RenderGraphTextureAccessに対応するTextureがRenderGraphResourceRegistryに登録されていません。");
		return;
	}

}

bool FWK::Graphics::RenderGraph::TransitionRenderTargetTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphResourceRegistry& a_renderGraphResourceRegistry, const DirectCommandList& a_directCommandList)
{
	const auto& l_renderTargetTextureResourceRecord = a_renderGraphResourceRegistry.FindVALRenderTargetTexture(a_textureAccess.m_textureTag).lock();

	if (!l_renderTargetTextureResourceRecord) { return false; }

	const auto& l_renderTargetTexture = l_renderTargetTextureResourceRecord->m_renderTargetTexture;

	if (!l_renderTargetTexture)
	{
		assert(false && "RenderGraph管理RenderTargetTextureが無効のため、状態遷移が出来ませんでした。");
		return true;
	}

	a_directCommandList.TransitionRenderTargetTexture(a_textureAccess.m_requiredState, *l_renderTargetTexture);

	return true;
}
bool FWK::Graphics::RenderGraph::TransitionDepthStencilTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphResourceRegistry& a_renderGraphResourceRegistry, const DirectCommandList& a_directCommandList)
{
	const auto& l_depthStencilTextureResourceRecord = a_renderGraphResourceRegistry.FindVALDepthStencilTexture(a_textureAccess.m_textureTag).lock();

	if (!l_depthStencilTextureResourceRecord) { return false; }

	const auto& l_depthStencilTexture = l_depthStencilTextureResourceRecord->m_depthStencilTexture;

	if (!l_depthStencilTexture)
	{
		assert(false && "RenderGraph管理DepthStencilTextureが無効のため、状態遷移が出来ませんでした。");
		return true;
	}

	a_directCommandList.TransitionDepthStencilTexture(a_textureAccess.m_requiredState, *l_depthStencilTexture);

	return true;
}