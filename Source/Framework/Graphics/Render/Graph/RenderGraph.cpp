#include "RenderGraph.h"

void FWK::Graphics::RenderGraph::INIT()
{
	m_passList.clear		   ();
	m_sortedPassIndexList.clear();

	m_drawRequestPassList.clear();
	m_drawRequestPassMap.clear ();

	m_drawRequestPerObjectList.clear();
	m_drawRequestPerObjectMap.clear ();
}

void FWK::Graphics::RenderGraph::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_renderGraphJsonConverter.Deserialize(a_rootJson, *this);
}

void FWK::Graphics::RenderGraph::PostCreateSetup(Renderer& a_renderer) const
{
	for (const auto& l_pass : m_passList)
	{
		FWK_ASSERT_RETURN_IF(!l_pass, "RenderGraphPassが無効のため、PostCreateSetupを実行できませんでした。")

		l_pass->PostCreateSetup(a_renderer);
	}
}

bool FWK::Graphics::RenderGraph::Compile()
{
	m_sortedPassIndexList.clear();

	const auto l_passCount = m_passList.size();

	FWK_ASSERT_RETURN_VALUE_IF(m_passList.empty(), "RenderGraphPassが登録されていないため、Compileに失敗しました。", false)

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
	FWK_ASSERT_RETURN_VALUE_IF(m_sortedPassIndexList.size() != l_passCount, "RenderGraphPassの依存関係が循環しているため、Compileに失敗しました。", false)

	return true;
}


void FWK::Graphics::RenderGraph::BeginFrame() const
{
	for (const auto& l_drawRequestPass : m_drawRequestPassList)
	{
		FWK_ASSERT_RETURN_IF(!l_drawRequestPass, "DrawRequestPassが無効のため、BeginFrameを実行できませんでした。")

		// 共通定数バッファのパラメータを更新
		l_drawRequestPass->BeginFrame();
	}

	for (const auto& l_drawRequestPerObject : m_drawRequestPerObjectList)
	{
		FWK_ASSERT_RETURN_IF		      (!l_drawRequestPerObject, "DrawRequestPerObjectが無効のため、BeginFrameを実行できませんでした。")
		l_drawRequestPerObject->BeginFrame();
	}
}

void FWK::Graphics::RenderGraph::Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap, 
										 const DSVDescriptorHeap&				  a_dsvDescriptorHeap, 
										 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
										 const SwapChain&						  a_swapChain, 
											   DirectCommandList&				  a_directCommandList,
											   Renderer&						  a_renderer)
{
	FWK_ASSERT_RETURN_IF(m_sortedPassIndexList.empty(), "RenderGraphがCompileされていないため、実行できませんでした。")

	for (const auto l_passIndex : m_sortedPassIndexList)
	{
		FWK_ASSERT_RETURN_IF(m_passList.size() <= l_passIndex, "RenderGraphPassIndexが範囲外です。")

		const auto& l_pass = m_passList[l_passIndex];

		FWK_ASSERT_RETURN_IF(!l_pass, "RenderGraphPassが無効です。")

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
						a_renderer,
						*this);
	}
}

nlohmann::json FWK::Graphics::RenderGraph::Serialize() const
{
	return m_renderGraphJsonConverter.Serialize(*this);
}

void FWK::Graphics::RenderGraph::AddPass(std::unique_ptr<IRenderGraphPass>&& a_pass)
{
	FWK_ASSERT_RETURN_IF(!a_pass, "RenderGraphPassが無効のため、追加できませんでした。")

	m_passList.emplace_back(std::move(a_pass));
}
void FWK::Graphics::RenderGraph::AddDrawRequestPass(const std::shared_ptr<DrawRequestPassBase>& a_drawRequestPass)
{
	FWK_ASSERT_RETURN_IF(!a_drawRequestPass, "DrawRequestPassが無効のため、RenderGraphへ追加できませんでした。")

	const auto l_staticTypeID = a_drawRequestPass->GetREFRuntimeTypeINFO().k_staticTypeID;

	FWK_ASSERT_RETURN_IF(m_drawRequestPassMap.contains(l_staticTypeID), "同じ型のDrawRequestPassを二重登録しようとしました。")

	m_drawRequestPassList.emplace_back(a_drawRequestPass);
	m_drawRequestPassMap.try_emplace  (l_staticTypeID, a_drawRequestPass);
}
void FWK::Graphics::RenderGraph::AddDrawRequestPerObject(const std::shared_ptr<DrawRequestPerObjectBase>& a_drawRequestPerObject)
{
	FWK_ASSERT_RETURN_IF(!a_drawRequestPerObject, "DrawRequestPerObjectが無効のため、RenderGraphへ追加できませんでした。")

	const auto l_staticTypeID = a_drawRequestPerObject->GetREFRuntimeTypeINFO().k_staticTypeID;

	FWK_ASSERT_RETURN_IF(m_drawRequestPerObjectMap.contains(l_staticTypeID), "同じ型のDrawRequestPerObjectを二重登録しようとしました。")

	m_drawRequestPerObjectList.emplace_back(a_drawRequestPerObject);
	m_drawRequestPerObjectMap.try_emplace  (l_staticTypeID, a_drawRequestPerObject);
}

bool FWK::Graphics::RenderGraph::IsReadAccess(const Struct::RenderGraphTextureAccess& a_textureAccess) const
{
	return a_textureAccess.m_accessTag == Utility::Tag::GetVALTag<Tag::RenderGraphReadAccessTag>();
}
bool FWK::Graphics::RenderGraph::IsWriteAccess(const Struct::RenderGraphTextureAccess& a_textureAccess) const
{
	return a_textureAccess.m_accessTag == Utility::Tag::GetVALTag<Tag::RenderGraphWriteAccessTag>();
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
	FWK_ASSERT_RETURN_IF(m_passList.empty(), "PassListが空です。")

	// Passが持っているTextureAccessを、依存関係を作りやすい形に変換して一つの配列へ集める
	// TextureAccessは「どのTextureTagを、Read/Writeのどちらで、どのUsageで使うか」を表す
	// 例 : 
	// LightingPass    : SceneColortextureTagをRenderTargetとしてWrite
	// PresentCopyPass : SceneColorTextureTagをCopySourceとしてRead
	std::vector<TextureAccessPassRecord> l_textureAccessPassRecordList = {};

	const auto l_passCount = m_passList.size();

	for (std::uint32_t l_passIndex = 0U; l_passIndex < l_passCount; ++l_passIndex)
	{
		const auto& l_pass = m_passList[l_passIndex];

		FWK_ASSERT_RETURN_IF(!l_pass, "RenderGraphPassが無効です。")

		for (const auto& l_textureAccess : l_pass->GetREFTextureAccessList())
		{
			// TextureTagは「どのTextureを使うか」を表す
			// これが無効だと、どのTexture動詞で依存関係を作ればいいか判断できない
			FWK_ASSERT_RETURN_IF(l_textureAccess.m_textureTag == Constant::k_invalidTypeTag, "RenderGraphTextureAccessのTextureTagが無効です。")

			// AccessTagは「ReadなのかWriteなのか」を表す
			// BuildDependencyでは、この情報を使ってPass同地の実行順を決める
			FWK_ASSERT_RETURN_IF(l_textureAccess.m_accessTag == Constant::k_invalidTypeTag, "RenderGraphTextureAccessのAccessTagが無効です。")
			
			// UsageTagは「RenderTargetとして使うのか、CopySourceとして使うのか」などの用途を表す
			// ResourceStateの自動遷移で使うため、ここで無効値を弾いておく
			FWK_ASSERT_RETURN_IF(l_textureAccess.m_usageTag == Constant::k_invalidTypeTag, "RenderGraphTextureAccessのUsageTagが無効です。")
			
			TextureAccessPassRecord l_textureAccessPassRecord = {};

			l_textureAccessPassRecord.m_textureTag = l_textureAccess.m_textureTag;
			l_textureAccessPassRecord.m_passIndex  = l_passIndex;

			// TextureTagで並べ替えた後も、元のPass登録順を維持するための番号
			// 同じTextureTagへ複数Writeする場合、この順番をWriteChainの順番として扱う
			l_textureAccessPassRecord.m_accessOrder = static_cast<std::uint32_t>(l_textureAccessPassRecordList.size());

			l_textureAccessPassRecord.m_isRead	= IsReadAccess (l_textureAccess);
			l_textureAccessPassRecord.m_isWrite = IsWriteAccess(l_textureAccess);

			// 書き込み、読み込み、どちらでもない場合return;
			FWK_ASSERT_RETURN_IF(!l_textureAccessPassRecord.m_isRead && !l_textureAccessPassRecord.m_isWrite, "RenderGraphTextureAccessのAccessTagが不正です。")


			l_textureAccessPassRecordList.emplace_back(l_textureAccessPassRecord);
		}
	}

	// TextureTagごとにAccessをまとめる
	// 例:
	// 並び替え前:
	//   SceneColor    / LightingPass / Write
	//   GBufferNormal / GBufferPass  / Write
	//   SceneColor    / UIPass       / Write
	//   GBufferNormal / LightingPass / Read
	//
	// 並び替え後:
	//   GBufferNormal / GBufferPass  / Write
	//   GBufferNormal / LightingPass / Read
	//   SceneColor    / LightingPass / Write
	//   SceneColor    / UIPass       / Write
	//
	// 同じTextureTagだけを連続して処理できるようになる
	std::stable_sort(l_textureAccessPassRecordList.begin(),
					 l_textureAccessPassRecordList.end(),
					 [](const auto& a_left, const auto& a_right){

						if (a_left.m_textureTag != a_right.m_textureTag)
						{
							return a_left.m_textureTag < a_right.m_textureTag;
						}

						return a_left.m_accessOrder < a_right.m_accessOrder;
					 });

	std::uint32_t l_groupBeginIndex = 0U;

	while (l_groupBeginIndex < static_cast<std::uint32_t>(l_textureAccessPassRecordList.size()))
	{
		std::uint32_t l_groupEndIndex = l_groupBeginIndex;

		// 同じTextureTagの範囲を探す
		// 範囲は[l_groupBeginINdex, l_groupEndIndex)
		// 例 : 
		// SceneColorのAccessが3個続いているなら、その3子だけを一つのグループとして処理する
		while (l_groupEndIndex < static_cast<std::uint32_t>(l_textureAccessPassRecordList.size()) &&
			   l_textureAccessPassRecordList[l_groupBeginIndex].m_textureTag == l_textureAccessPassRecordList[l_groupEndIndex].m_textureTag)
		{
			++l_groupEndIndex;
		}

		// このTextureTagに最後にWriteしたPassIndex
		// 無効値の場合は、まだこのTextureTagへWriteしたPassが存在しない
		std::uint32_t l_lastWritePassIndex = k_invalidRenderGraphPassIndex;

		// 次のWritePassより前に終わっている必要があるReadPass一覧
		// Read -> Writeの依存関係を作るために使う
		std::vector<std::uint32_t> l_pendingReadPassIndexList = {};

		for (std::uint32_t l_recordIndex = l_groupBeginIndex; l_recordIndex < l_groupEndIndex; ++l_recordIndex)
		{
			const auto& l_textureAccessPassRecord = l_textureAccessPassRecordList[l_recordIndex];

			if (l_textureAccessPassRecord.m_isRead)
			{
				if (l_lastWritePassIndex != k_invalidRenderGraphPassIndex)
				{
					// 最後にWriteしたPassの結果を、このReadPassが読む
					// 例:
					// UIPass      : SceneColorへWrite
					// PresentPass : SceneColorをRead
					// この場合、UIPass -> PresentPassの依存関係を作る
					AddDependencyEdge(l_lastWritePassIndex,
									  l_textureAccessPassRecord.m_passIndex,
									  a_edgeList,
									  a_inDegreeList);
				}

				bool l_hasSameReadPassIndex = false;

				for (const auto l_pendingReadPassIndex : l_pendingReadPassIndexList)
				{
					if (l_pendingReadPassIndex != l_textureAccessPassRecord.m_passIndex) { continue; }

					l_hasSameReadPassIndex = true;
					break;
				}

				if (!l_hasSameReadPassIndex)
				{
					// このReadPassの後に同じTextureTagへWriteするPassが来た場合、
					// Read中にTextureを書き換えないように ReadPass -> WritePass の依存関係を作る
					// ここではまだ次のWritePassが分からないため、一旦pendingとして保存する
					l_pendingReadPassIndexList.emplace_back(l_textureAccessPassRecord.m_passIndex);
				}

				continue;
			}

			if (l_textureAccessPassRecord.m_isWrite)
			{
				if (l_lastWritePassIndex != k_invalidRenderGraphPassIndex)
				{
					// 同じTextureTagへ連携してWriteする場合、
					// 登録順をWriteChainとして扱う
					// 例 : 
					// LightingPass -> TransparentPass -> UIPass
					AddDependencyEdge(l_lastWritePassIndex,
									  l_textureAccessPassRecord.m_passIndex,
									  a_edgeList,
									  a_inDegreeList);
				}

				for (const auto l_pendingReadPassIndex : l_pendingReadPassIndexList)
				{
					// 前に同じTextureTagをReadしていたPassがある場合、
					// そのReadPassが終わってからWriteする
					// 例:
					// HistoryReadPass   : HistoryTextureをRead
					// HistoryUpdatePass : HistoryTextureへWrite
					// この場合、HistoryReadPass -> HistoryUpdatePassの依存関係を作る
					AddDependencyEdge(l_pendingReadPassIndex,
									  l_textureAccessPassRecord.m_passIndex,
									  a_edgeList,
									  a_inDegreeList);
				}

				l_pendingReadPassIndexList.clear();

				// 今回のWritePassIndexを、このTextureTagの最後のWritePassとして記録する
				l_lastWritePassIndex = l_textureAccessPassRecord.m_passIndex;

				continue;
			}
		}

		l_groupBeginIndex = l_groupEndIndex;
	}
}

void FWK::Graphics::RenderGraph::TransitionPassTexture(const IRenderGraphPass& a_pass, DirectCommandList& a_directCommandList, Renderer& a_renderer)
{
	const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "FrameResourceが無効のため、RenderGraphPass用Textureの状態遷移ができませんでした。")

	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();

	for (const auto& l_textureAccess : a_pass.GetREFTextureAccessList())
	{
		if (TransitionRenderTargetTexture(l_textureAccess, l_renderGraphResourceRegistry, a_directCommandList)) { continue; }
		if (TransitionDepthStencilTexture(l_textureAccess, l_renderGraphResourceRegistry, a_directCommandList)) { continue; }

		FWK_ASSERT_RETURN("RenderGraphTextureAccessに対応するTextureがRenderGraphResourceRegistryに登録されていません。")
	}

}

bool FWK::Graphics::RenderGraph::TransitionRenderTargetTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry, const DirectCommandList& a_directCommandList)
{
	const auto& l_renderTargetTextureResourceRecord = a_renderGraphFrameResourceRegistry.FindVALRenderTargetTexture(a_textureAccess.m_textureTag).lock();

	if (!l_renderTargetTextureResourceRecord) { return false; }

	const auto& l_renderTargetTexture = l_renderTargetTextureResourceRecord->m_renderTargetTexture;

	FWK_ASSERT_RETURN_VALUE_IF(!l_renderTargetTexture, "RenderGraph管理RenderTargetTextureが無効のため、状態遷移が出来ませんでした。", true)

	const auto l_requiredState = ConvertTextureUsageToResourceState(a_textureAccess.m_usageTag);

	a_directCommandList.TransitionRenderTargetTexture(l_requiredState, *l_renderTargetTexture);

	return true;
}
bool FWK::Graphics::RenderGraph::TransitionDepthStencilTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry, const DirectCommandList& a_directCommandList)
{
	const auto& l_depthStencilTextureResourceRecord = a_renderGraphFrameResourceRegistry.FindVALDepthStencilTexture(a_textureAccess.m_textureTag).lock();

	if (!l_depthStencilTextureResourceRecord) { return false; }

	const auto& l_depthStencilTexture = l_depthStencilTextureResourceRecord->m_depthStencilTexture;

	FWK_ASSERT_RETURN_VALUE_IF(!l_depthStencilTexture, "RenderGraph管理DepthStencilTextureが無効のため、状態遷移が出来ませんでした。", true)

	const auto l_requiredState = ConvertTextureUsageToResourceState(a_textureAccess.m_usageTag);

	a_directCommandList.TransitionDepthStencilTexture(l_requiredState, *l_depthStencilTexture);

	return true;
}

D3D12_RESOURCE_STATES FWK::Graphics::RenderGraph::ConvertTextureUsageToResourceState(const TypeAlias::TypeTag a_usageTag) const
{
	if (a_usageTag == Utility::Tag::GetVALTag<Tag::RenderGraphRenderTargetUsageTag>())
	{
		// RenderTargetとして書き込むためのResourceState
		return D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	if (a_usageTag == Utility::Tag::GetVALTag<Tag::RenderGraphCopySourceUsageTag>())
	{
		// CopyTextureRegionなどでコピー元として読むためのResourceState
		return D3D12_RESOURCE_STATE_COPY_SOURCE;
	}

	if (a_usageTag == Utility::Tag::GetVALTag<Tag::RenderGraphShaderReadUsageTag>())
	{
		// PixelShader / MeshShader / ComputeShaderなど、シェーダーから読むためのResourceState
		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	}

	if (a_usageTag == Utility::Tag::GetVALTag<Tag::RenderGraphDepthWriteUsageTag>())
	{
		// DepthStencilとして深度を書き込むためのResourceState
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}

	if (a_usageTag == Utility::Tag::GetVALTag<Tag::RenderGraphDepthReadUsageTag>())
	{
		// DepthStencilとして深度を読むためのResourceState
		return D3D12_RESOURCE_STATE_DEPTH_READ;
	}

	FWK_ASSERT_RETURN_VALUE("RenderGraphTextureAccessのUsageTagが不正です。", D3D12_RESOURCE_STATE_COMMON)
}