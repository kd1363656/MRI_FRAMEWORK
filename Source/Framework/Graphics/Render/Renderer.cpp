#include "Renderer.h"

void FWK::Graphics::Renderer::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_rendererJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::Renderer::Create(const Device& a_device, const ShaderCompiler& a_shaderCompiler)
{
	for (auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource.Create(a_device))
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

	for (auto& [l_key, l_value] : m_rootSignatureMap)
	{
		if (!l_value.Create(a_device))
		{
			assert(false && "ルートシグネチャの作成に失敗しました。");
			return false;
		}
	}

	for (auto& [l_key, l_value] : m_pipelineStateMap)
	{
		if (!l_value.Create(a_device, a_shaderCompiler, *this))
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
}

void FWK::Graphics::Renderer::BeginDraw(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap)
{
	const auto* const l_currentFrameResource = FetchPTRCurrentFrameResource();

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
	m_directCommandList.TransitionRenderTargetResource(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, a_swapChain);

	// 今回使用するバックバッファを設定
	m_directCommandList.SetupBackBuffer(a_swapChain, a_rtvDescriptorHeap);

	// ビューポートとシザー矩形を設定
	m_directCommandList.SetupRenderArea(m_renderArea);
}

void FWK::Graphics::Renderer::SetupGraphicsPipelineByTag(const TypeAlias::TypeTag a_pipelineStateTag) const
{
	auto* l_pipelineState = FindPTRPipelineState(a_pipelineStateTag);

	if (!l_pipelineState) 
	{
		assert(false && "使用するパイプラインステートが作成されておらず、描画を開始できませんでした。");
		return; 
	}

	// パイプラインステートが使用するルートシグネチャを取得
	auto* l_rootSignature = FindPTRRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

	if (!l_rootSignature)
	{
		assert(false && "使用するルートシグネチャが作成されておらず、描画を開始できませんでした。");
		return;
	}

	m_directCommandList.SetupRootSignature(l_rootSignature);

	// パイプラインステートをセット
	m_directCommandList.SetupPipelineState(l_pipelineState);
}

void FWK::Graphics::Renderer::Draw() const
{
	// スプライト病
	SetupGraphicsPipelineByTag(Utility::Tag::GetTag<Tag::SpriteStandardPipelineStateTag>());

	m_directCommandList.DispatchMesh(k_defaultDispatchMeshThreadGroupCountX, k_defaultDispatchMeshThreadGroupCountY, k_defaultDispatchMeshThreadGroupCountZ);
}
void FWK::Graphics::Renderer::EndDraw(const SwapChain& a_swapChain)
{
	auto* l_currentFrameResource = FetchMutablePTRCurrentFrameResource();

	if (!l_currentFrameResource)
	{
		assert(false && "フレームリソースの取得に失敗しており、描画終了処理を行うことができませんでした。");
		return;
	}

	auto& l_commandAllocator = l_currentFrameResource->GetMutableREFDirectCommandAllocator();

	// バックバッファの状態遷移(RESOURCE -> PRESENT)
	m_directCommandList.TransitionRenderTargetResource(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, a_swapChain);

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
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + k_incrementCurrentFrameIndex) % static_cast<UINT>(m_frameResourceList.size());
}

nlohmann::json FWK::Graphics::Renderer::Serialize() const
{
	return m_rendererJsonConverter.Serialize(*this);
}

void FWK::Graphics::Renderer::AddRootSignature(const RootSignature& a_rootSignature, const TypeAlias::TypeTag a_tag)
{
	m_rootSignatureMap.try_emplace(a_tag, a_rootSignature);
}
void FWK::Graphics::Renderer::AddPipelineState(const PipelineState& a_pipelineState, const TypeAlias::TypeTag a_tag)
{
	m_pipelineStateMap.try_emplace(a_tag, a_pipelineState);
}

const FWK::Graphics::RootSignature* FWK::Graphics::Renderer::FindPTRRootSignature(const TypeAlias::TypeTag a_tag) const
{
	const auto& l_itr = m_rootSignatureMap.find(a_tag);

	if (l_itr == m_rootSignatureMap.end()) { return nullptr; }

	return &l_itr->second;
}
const FWK::Graphics::PipelineState* FWK::Graphics::Renderer::FindPTRPipelineState(const TypeAlias::TypeTag a_tag) const
{
	const auto& l_itr = m_pipelineStateMap.find(a_tag);

	if (l_itr == m_pipelineStateMap.end()) { return nullptr; }

	return &l_itr->second;
}

const FWK::Graphics::FrameResource* FWK::Graphics::Renderer::FetchPTRCurrentFrameResource() const
{
	if (m_currentFrameResourceIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのため、フレームリソース取得が行えませんでした。");
		return nullptr;
	}

	return &m_frameResourceList[m_currentFrameResourceIndex];
}

FWK::Graphics::FrameResource* FWK::Graphics::Renderer::FetchMutablePTRCurrentFrameResource()
{
	if (m_currentFrameResourceIndex >= static_cast<UINT>(m_frameResourceList.size()))
	{
		assert(false && "フレームリソースの容量を超えたインデックスのため、フレームリソース取得が行えませんでした。");
		return nullptr;
	}

	return &m_frameResourceList[m_currentFrameResourceIndex];
}