#include "Renderer.h"

void FWK::Graphics::Renderer::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_rendererJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::Renderer::Create(const Device& a_device, const ShaderCompiler& a_shaderCompiler)
{
	for (const auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource) { continue; }

		if (!l_frameResource->Create(a_device))
		{
			assert(false && "フレームリソースの作成処理に失敗しました。");
			return false;
		}
	}

	if (!m_directCommandQueue.Create(a_device))
	{
		assert(false && "ダイレクトコマンドキューの作成処理に失敗しました。");
		return false;
	}

	if (!m_directCommandList.Create(a_device))
	{
		assert(false && "ダイレクトコマンドリストの作成処理に失敗しました。");
		return false;
	}

	for (const auto& [l_tag, l_rootSignature] : m_rootSignatureMap)
	{
		if (!l_rootSignature) 
		{
			assert(false && "RootSignatureが無効のため、RootSignatureの作成に失敗しました。");
			return false;
		}

		if (!l_rootSignature->Create(a_device))
		{
			assert(false && "ルートシグネチャの作成に失敗しました。");
			return false;
		}
	}

	for (const auto& [l_tag, l_pipelineState] : m_pipelineStateMap)
	{
		if (!l_pipelineState)
		{
			assert(false && "PipelineStateが無効のため、PipelineStateの作成に失敗しました。");
			return false;
		}
		
		if (!l_pipelineState->Create(a_device, a_shaderCompiler, *this))
		{
			assert(false && "パイプラインステートの作成に失敗しました。");
			return false;
		}
	}

	return true;
}
void FWK::Graphics::Renderer::PostCreateSetup(const SwapChain& a_swapChain)
{
	m_renderArea.SetupRenderArea(a_swapChain);

	for (const auto& l_drawCommand : m_drawCommandList)
	{
		if (!l_drawCommand) { continue; }

		// 描画コマンドで使用するルートシグネチャやパイプラインステート設定する
		l_drawCommand->PostCreateSetup(*this);
	}
}

void FWK::Graphics::Renderer::BeginFrame() const
{
	// 前フレームの描画申請を削除、現フレームで描画する必要のあるものだけ取り入れるようにする
	for (const auto& l_drawCommand : m_drawCommandList)
	{
		if (!l_drawCommand) { continue; }

		l_drawCommand->BeginFrame();
	}
}

void FWK::Graphics::Renderer::BeginDraw(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap)
{
	const auto& l_currentFrameResource = FetchVALCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "フレームリソースの取得に失敗しており、描画開始処理を行うことができませんでした。");
		return;
	}

	const auto& l_commandAllocator = l_currentFrameResource->GetREFDirectCommandAllocator();

	// コマンドアロケータからGPU処理が終わっているかどうかを確かめGPUの処理が終わっていなければWait
	m_directCommandQueue.EnsureAllocatorAvailable(l_commandAllocator);

	// GPU同期処理が終わってからコマンドリスト、アロケータをリセット
	l_commandAllocator.Reset ();
	m_directCommandList.Reset(l_commandAllocator);

	// バックバッファの状態遷移(PRESENT -> RESOURCE)
	m_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 今回使用するバックバッファを設定
	m_directCommandList.SetupBackBuffer(a_swapChain, a_rtvDescriptorHeap);

	// ビューポートとシザー矩形を設定
	m_directCommandList.SetupRenderArea(m_renderArea);
}

void FWK::Graphics::Renderer::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	for (const auto& l_drawCommand : m_drawCommandList)
	{
		if (!l_drawCommand) { continue; }

		l_drawCommand->Draw(a_srvDescriptorPool, *this);
	}
}
void FWK::Graphics::Renderer::EndDraw(const SwapChain& a_swapChain)
{
	const auto& l_currentFrameResource = FetchVALCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "フレームリソースの取得に失敗しており、描画終了処理を行うことができませんでした。");
		return;
	}

	auto& l_commandAllocator = l_currentFrameResource->GetMutableREFDirectCommandAllocator();

	// バックバッファの状態遷移(RESOURCE -> PRESENT)
	m_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストへの命令記録を終了
	m_directCommandList.Close();

	// 描画命令を実行
	m_directCommandQueue.ExecuteCommandLists(m_directCommandList);

	// フェンス値を更新
	m_directCommandQueue.SignalAndTrackAllocator(l_commandAllocator);

	a_swapChain.Present();
}
void FWK::Graphics::Renderer::EndFrame()
{
	// 容量を超えないように次のフレームで使用するインデックスを計算
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + k_frameResourceIndexIncrement) % m_frameResourceList.size();
}

nlohmann::json FWK::Graphics::Renderer::Serialize() const
{
	return m_rendererJsonConverter.Serialize(*this);
}

void FWK::Graphics::Renderer::AddFrameResource(const std::shared_ptr<FrameResource>& a_frameResource)
{
	if (!a_frameResource) 
	{
		assert(false && "FrameResourceが無効のため、FrameResourceListへの登録に失敗しました。");
		return; 
	}

	m_frameResourceList.emplace_back(a_frameResource);
}
void FWK::Graphics::Renderer::AddDrawCommandList(const std::shared_ptr<DrawCommandBase>& a_drawCommand)
{
	if (!a_drawCommand) 
	{
		assert(false && "DrawCommandが無効のため、DrawCommandListへの登録に失敗しました。");
		return;
	}

	m_drawCommandList.emplace_back(a_drawCommand);
}
void FWK::Graphics::Renderer::AddDrawCommandMap(const std::shared_ptr<DrawCommandBase>& a_drawCommand, const TypeAlias::StaticTypeID a_staticTypeID)
{
	if (!a_drawCommand)
	{
		assert(false && "DrawCommandが無効のため、DrawCommandMapへの登録に失敗しました。");
		return;
	}

	m_drawCommandMap.try_emplace(a_staticTypeID, a_drawCommand);
}
void FWK::Graphics::Renderer::AddRootSignature(const std::shared_ptr<RootSignature>& a_rootSignature, const TypeAlias::TypeTag a_tag)
{
	if (!a_rootSignature)
	{
		assert(false && "RootSignatureが無効のため、RootSignatureMapへの登録に失敗しました。");
		return;
	}

	m_rootSignatureMap.try_emplace(a_tag, a_rootSignature);
}
void FWK::Graphics::Renderer::AddPipelineState(const std::shared_ptr<PipelineState>& a_pipelineState, const TypeAlias::TypeTag a_tag)
{
	if (!a_pipelineState)
	{
		assert(false && "PipelineStateが無効のため、PipelineStateMapへの登録に失敗しました。");
		return;
	}

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

std::weak_ptr<FWK::Graphics::FrameResource> FWK::Graphics::Renderer::FetchVALCurrentFrameResource() const
{
	if (m_frameResourceList.empty())
	{
		assert(false && "フレームリソースが空のため、フレームリソース取得が行えませんでした。");
		return {};
	}

	if (m_currentFrameResourceIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのため、フレームリソース取得が行えませんでした。");
		return {};
	}

	return m_frameResourceList[m_currentFrameResourceIndex];
}