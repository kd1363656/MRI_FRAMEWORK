#include "Renderer.h"

void FWK::Graphics::Renderer::INIT()
{
	m_lightSystem.ApplyDefaultSettings();
}
void FWK::Graphics::Renderer::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_rendererJsonConverter.Deserialize(a_rootJson, *this);
}
void FWK::Graphics::Renderer::PostDeserializeSetup(const Struct::WindowCONFIG& a_windowCONFIG)
{
	for (const auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource) { continue; }

		l_frameResource->PostDeserializeSetup(a_windowCONFIG);
	}
}
bool FWK::Graphics::Renderer::Create(const Device&							  a_device, 
									 const ShaderCompiler&					  a_shaderCompiler, 
									 const GPUMemoryAllocator&				  a_gpuMemoryAllocator,
									 const UINT								  a_width,
									 const UINT								  a_height, 
										   DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool, 
										   DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
										   DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	for (const auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource) { continue; }

		FWK_ASSERT_RETURN_VALUE_IF(!l_frameResource->Create(a_device, 
									 a_gpuMemoryAllocator,
									 a_width,
									 a_height,
									 a_rtvDescriptorPool,
									 a_srvDescriptorPool,
									 a_dsvDescriptorPool),
									 "フレームリソースの作成処理に失敗しました。",
									 false)
	}

	FWK_ASSERT_RETURN_VALUE_IF(!m_directCommandQueue.Create(a_device), "ダイレクトコマンドキューの作成処理に失敗しました。", false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_directCommandList.Create(a_device),  "ダイレクトコマンドリストの作成処理に失敗しました。", false)

	for (const auto& [l_tag, l_rootSignature] : m_rootSignatureMap)
	{
		FWK_ASSERT_RETURN_VALUE_IF(!l_rootSignature,				   "RootSignatureが無効のため、RootSignatureの作成に失敗しました。", false)
		FWK_ASSERT_RETURN_VALUE_IF(!l_rootSignature->Create(a_device), "ルートシグネチャの作成に失敗しました。",						     false)
	}

	for (const auto& [l_tag, l_pipelineState] : m_pipelineStateMap)
	{
		FWK_ASSERT_RETURN_VALUE_IF(!l_pipelineState,											"PipelineStateが無効のため、PipelineStateの作成に失敗しました。", false)
		FWK_ASSERT_RETURN_VALUE_IF(!l_pipelineState->Create(a_device, a_shaderCompiler, *this), "パイプラインステートの作成に失敗しました。",					  false)
	}

	return true;
}
void FWK::Graphics::Renderer::PostCreateSetup(const SwapChain& a_swapChain)
{
	m_renderArea.SetupRenderArea(a_swapChain);

	m_renderGraph.PostCreateSetup(*this);

	// 定数バッファに各パス情報を送る
	m_lightSystem.SyncLightPassDrawRequest();
	m_renderArea.SyncSpritePassDrawRequest();

	FWK_ASSERT_RETURN_IF(!m_renderGraph.Compile(), "RenderGraphのCompileに失敗しました。")
}

void FWK::Graphics::Renderer::BeginFrame()
{
	// 現在のフレームリソースの定数バッファのインデックスの初期化
	const auto& l_currentFrameResource = m_currentFrameResource.lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "フレームリソースの取得に失敗しており、描画終了処理を行うことができませんでした。")

	l_currentFrameResource->BeginFrame();

	m_renderGraph.BeginFrame();
}

void FWK::Graphics::Renderer::BeginDraw()
{
	const auto& l_currentFrameResource = m_currentFrameResource.lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "フレームリソースの取得に失敗しており、描画開始処理を行うことができませんでした。")

	const auto& l_commandAllocator = l_currentFrameResource->GetREFDirectCommandAllocator();

	FWK_ASSERT_RETURN_IF(!l_commandAllocator, "ダイレクトコマンドアロケータが無効になっており、描画開始処理を行うことができませんでした。")
	
	// コマンドアロケータからGPU処理が終わっているかどうかを確かめGPUの処理が終わっていればWait
	m_directCommandQueue.EnsureAllocatorAvailable(*l_commandAllocator);

	// GPU同期処理が終わってからコマンドリスト、アロケータをリセット
	l_commandAllocator->Reset();
	m_directCommandList.Reset(*l_commandAllocator);
}

void FWK::Graphics::Renderer::EndDraw(const SwapChain& a_swapChain)
{
	const auto& l_currentFrameResource = m_currentFrameResource.lock();

	FWK_ASSERT_RETURN_IF(!l_currentFrameResource, "フレームリソースの取得に失敗しており、描画終了処理を行うことができませんでした。")

	const auto& l_commandAllocator = l_currentFrameResource->GetMutableREFDirectCommandAllocator();

	FWK_ASSERT_RETURN_IF(!l_commandAllocator, "ダイレクトコマンドアロケータが無効になっています。")

	// IMGUIの描画処理の関係上ここでRENDER_TARGET -> PRESENTにここで遷移する
	// BackBufferを画面表示できる状態に戻す
	m_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストへの命令記録を終了
	m_directCommandList.Close();

	// 描画命令を実行
	m_directCommandQueue.ExecuteCommandLists(m_directCommandList);

	// フェンス値を更新
	m_directCommandQueue.SignalAndTrackAllocator(*l_commandAllocator);

	a_swapChain.Present();

}
void FWK::Graphics::Renderer::EndFrame()
{
	// 容量を超えないように次のフレームで使用するインデックスを計算
	// 現在のインデックス / フレームリソースの総数の余りを算出しているので
	// 絶対にインデックスのオーバーフローが起きないことは保証されている
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + k_frameResourceIndexIncrement) % m_frameResourceList.size();

	// 次に使用するフレームリソースをキャッシュしておく
	m_currentFrameResource = m_frameResourceList[m_currentFrameResourceIndex];
}

void FWK::Graphics::Renderer::ExecuteRenderGraph(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
												 const RTVDescriptorHeap&				  a_rtvDescriptorHeap, 
												 const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
												 const SwapChain&						  a_swapChain)
{
	m_renderGraph.Execute(a_rtvDescriptorHeap,
						  a_dsvDescriptorHeap,
						  a_srvDescriptorPool,
						  a_swapChain,
						  m_directCommandList,
						  *this);
}

nlohmann::json FWK::Graphics::Renderer::Serialize() const
{
	return m_rendererJsonConverter.Serialize(*this);
}

void FWK::Graphics::Renderer::SetupCurrentFrameResource(const std::size_t& a_index)
{
	FWK_ASSERT_RETURN_IF(m_frameResourceList.empty(),		    "フレームリソースリストが空になっており、現在使用するフレームリソースを設定できませんでした。")
	FWK_ASSERT_RETURN_IF(m_frameResourceList.size() <= a_index, "フレームリソースの要素数を超えておりフレームリソースを設定できませんでした。" )

	m_currentFrameResourceIndex = a_index;
	m_currentFrameResource      = m_frameResourceList[m_currentFrameResourceIndex];
}

bool FWK::Graphics::Renderer::Resize(const Device&							  a_device,
									 const GPUMemoryAllocator&				  a_gpuMemoryAllocator,
									 const Struct::ClientSize&				  a_clientSize,
									 const UINT64&							  a_retiredFenceValue,
									 	   DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
									 	   DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
									 	   DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool,
									 	   DeferredResourceReleaseQueue&	  a_deferredResourceReleaseQueue) const
{
	for (const auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource) { continue; }

		FWK_ASSERT_RETURN_VALUE_IF(!l_frameResource->Resize(a_device, 
															a_gpuMemoryAllocator,
															a_clientSize,
															a_retiredFenceValue,
															a_rtvDescriptorPool,
															a_srvDescriptorPool,
															a_dsvDescriptorPool,
															a_deferredResourceReleaseQueue),
															"FrameResourceのリサイズに失敗しました。",
															false)
	}

	return true;
}


bool FWK::Graphics::Renderer::PrepareForSwapChainResize()
{
	// ResizeBuffers()の前に、GPUが直前までの描画命令を使い終わっている必要がある、
	// ここでは最後にSignalしたFenceまで待機して、GPU側のBackBuffer使用が終わるのを待つ。
	m_directCommandQueue.WaitForGPUIdleIfNeeded();

	// フレームリソースにバックバッファ情報を残していてはいけないので全てのフレームリソースに対して
	// リセット処理を行う
	for (const auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource) { continue; }

		const auto& l_commandAllocator = l_frameResource->GetREFDirectCommandAllocator();

		FWK_ASSERT_RETURN_VALUE_IF(!l_commandAllocator, "ダイレクトコマンドアロケータが無効のため、スワップチェインリサイズ前処理を行えませんでした。", false)

		// コマンドアロケータをリセット
		// コマンドアロケータは、コマンドリストに記録した命令のメモリを管理するもの
		// GPU待機後なので安全に利用可能
		l_commandAllocator->Reset();
	}

	
	const auto& l_currentFrameResource = m_currentFrameResource.lock();

	FWK_ASSERT_RETURN_VALUE_IF(!l_currentFrameResource, "フレームリソースの取得に失敗しており、描画終了処理を行うことができませんでした。", false)

	const auto& l_commandAllocator = l_currentFrameResource->GetREFDirectCommandAllocator();

	FWK_ASSERT_RETURN_VALUE_IF(!l_commandAllocator, "ダイレクトコマンドアロケータが無効のため、スワップチェインリサイズ前処理を行えませんでした。", false)

	// DirectCommandListをリセット
	// これにより、前フレームで記録したBackBufferへのResourceBarrierなどの参照を外す。
	m_directCommandList.Reset(*l_commandAllocator);

	// Resetした直後のコマンドリストは「記録中」の状態になる。
	// このままにすると次のBeginDraw()で再度Reset出来なくなるため、空のままCloseしておく
	m_directCommandList.Close();

	return true;
}

void FWK::Graphics::Renderer::AddFrameResource(const std::shared_ptr<FrameResource>& a_frameResource)
{
	FWK_ASSERT_RETURN_IF(!a_frameResource, "FrameResourceが無効のため、FrameResourceListへの登録に失敗しました。")

	m_frameResourceList.emplace_back(a_frameResource);
}
void FWK::Graphics::Renderer::AddRootSignature(const std::shared_ptr<RootSignature>& a_rootSignature, const TypeAlias::TypeTag a_tag)
{
	FWK_ASSERT_RETURN_IF(!a_rootSignature, "RootSignatureが無効のため、RootSignatureMapへの登録に失敗しました。")

	m_rootSignatureMap.try_emplace(a_tag, a_rootSignature);
}
void FWK::Graphics::Renderer::AddPipelineState(const std::shared_ptr<PipelineState>& a_pipelineState, const TypeAlias::TypeTag a_tag)
{
	FWK_ASSERT_RETURN_IF(!a_pipelineState, "PipelineStateが無効のため、PipelineStateMapへの登録に失敗しました。")

	m_pipelineStateMap.try_emplace(a_tag, a_pipelineState);
}

std::weak_ptr<FWK::Graphics::RootSignature> FWK::Graphics::Renderer::FindVALRootSignature(const TypeAlias::TypeTag a_tag) const
{
	const auto& l_itr = m_rootSignatureMap.find(a_tag);

	if (l_itr == m_rootSignatureMap.end()) { return {}; }

	return l_itr->second;
}
std::weak_ptr<FWK::Graphics::PipelineState> FWK::Graphics::Renderer::FindVALPipelineState(const TypeAlias::TypeTag a_tag) const
{
	const auto& l_itr = m_pipelineStateMap.find(a_tag);

	if (l_itr == m_pipelineStateMap.end()) { return {}; }

	return l_itr->second;
}