#include "Renderer.h"

void FWK::Graphics::Renderer::INIT()
{
	m_lightSystem.ApplyDefaultLightDirection();
	m_lightSystem.ApplyDefaultAmbientLight  ();
}
void FWK::Graphics::Renderer::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_rendererJsonConverter.Deserialize(a_rootJson, *this);
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

		if (!l_frameResource->Create(a_device, 
									 a_gpuMemoryAllocator,
									 a_width,
									 a_height,
									 a_rtvDescriptorPool,
									 a_srvDescriptorPool,
									 a_dsvDescriptorPool))
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

	m_renderGraph.PostCreateSetup(*this);

	if (!m_renderGraph.Compile())
	{
		assert(false && "RenderGraphのCompileに失敗しました。");
		return;
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

void FWK::Graphics::Renderer::BeginDraw()
{
	const auto& l_currentFrameResource = m_currentFrameResource.lock();

	if (!l_currentFrameResource)
	{
		assert(false && "フレームリソースの取得に失敗しており、描画開始処理を行うことができませんでした。");
		return;
	}

	const auto& l_commandAllocator = l_currentFrameResource->GetREFDirectCommandAllocator();

	if (!l_commandAllocator) 
	{
		assert(false && "ダイレクトコマンドアロケータが無効になっており、描画開始処理を行うことができませんでした。");
		return;
	}

	// コマンドアロケータからGPU処理が終わっているかどうかを確かめGPUの処理が終わっていればWait
	m_directCommandQueue.EnsureAllocatorAvailable(*l_commandAllocator);

	// GPU同期処理が終わってからコマンドリスト、アロケータをリセット
	l_commandAllocator->Reset();
	m_directCommandList.Reset(*l_commandAllocator);
}

void FWK::Graphics::Renderer::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	for (const auto& l_drawCommand : m_drawCommandList)
	{
		if (!l_drawCommand) { continue; }

		l_drawCommand->BeginDraw(a_srvDescriptorPool, *this);
		l_drawCommand->Draw	    (*this);
	}
}
void FWK::Graphics::Renderer::EndDraw(const SwapChain& a_swapChain)
{
	const auto& l_currentFrameResource = m_currentFrameResource.lock();

	if (!l_currentFrameResource)
	{
		assert(false && "フレームリソースの取得に失敗しており、描画終了処理を行うことができませんでした。");
		return;
	}

	const auto& l_commandAllocator = l_currentFrameResource->GetMutableREFDirectCommandAllocator();

	if (!l_commandAllocator)
	{
		assert(false && "ダイレクトコマンドアロケータが無効になっています。");
		return;
	}

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
	if (m_frameResourceList.empty())
	{
		assert(false && "フレームリソースリストが空になっており、現在使用するフレームリソースを設定できませんでした。");
		return;
	}

	if (m_frameResourceList.size() <= a_index)
	{
		assert(false && "フレームリソースの要素数を超えておりフレームリソースを設定できませんでした。");
		return;
	}

	m_currentFrameResourceIndex = a_index;
	m_currentFrameResource      = m_frameResourceList[m_currentFrameResourceIndex];
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