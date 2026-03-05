#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList(const Device& a_device) : 
	CommandListBase(a_device, D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;

void FWK::Graphics::DirectCommandList::TransitionRenderTargetResource(const SwapChain& a_swapChain, const D3D12_RESOURCE_STATES a_beforState, const D3D12_RESOURCE_STATES a_afterState) const
{
	const auto& l_backBufferList = a_swapChain.GetBackBufferList();
	
	if (l_backBufferList.empty())
	{
		assert(false && "バックバッファリストの要素が存在せず、BeginFrame処理が行えませんでした。");
		return;
	}
	
	const UINT l_currentBackBufferIndex = a_swapChain.GetCurrentBackBufferIndex();
	
	if (l_backBufferList.size() <= l_currentBackBufferIndex)
	{
		assert(false && "バックバッファの要素数を超えており、BeginFrame処理が行えませんでした。");
		return;
	}
	
	// リソースの状態遷移(Present -> RenderTarget)
	TransitionResource(l_backBufferList[l_currentBackBufferIndex], a_beforState, a_afterState);
}

void FWK::Graphics::DirectCommandList::SetupRenderTarget(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetCommandList().Get();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、描画コマンドを発行出来ませんでした。");
		return;
	}

	const auto  l_currentBackBufferIndex = a_swapChain.GetCurrentBackBufferIndex();

	// バックバッファーの現在のインデックスに対応したハンドルを取得
	auto l_rtvH = a_rtvDescriptorHeap.FetchCPUHandle(l_currentBackBufferIndex);

	// OMステージにレンダーターゲットをバインド
	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNum,
								            &l_rtvH,
								            true,
								            nullptr);

	// 現在設定されているレンダーターゲットを所定の色でクリア
	l_directCommandList->ClearRenderTargetView(l_rtvH,
											   k_clearColor, 
											   k_executeClearRectNum,
											   nullptr);
}
void FWK::Graphics::DirectCommandList::SetupRenderArea(const RenderArea& a_renderArea) const
{
	const auto& l_directCommandList = GetCommandList().Get();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ビューポートとシザー矩形の設定が出来ませんでした。");
		return;
	}

	// グラフィックスコマンドリストにビューポートとシザー矩形をセット
	l_directCommandList->RSSetViewports   (k_setViewportNum,    &a_renderArea.GetViewport());
	l_directCommandList->RSSetScissorRects(k_setScissorRectNum, &a_renderArea.GetScissorRect());
}
void FWK::Graphics::DirectCommandList::SetupRootSignature(const RootSignature& a_rootSignature) const
{
	const auto& l_directCommandList = GetCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

	const auto& l_rootSignature = a_rootSignature.GetRootSignature();

	if (!l_rootSignature)
	{
		assert(false && "ルートシグネチャが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}
	
	// コマンドリストにルートシグネチャをセット
	l_directCommandList->SetGraphicsRootSignature(l_rootSignature.Get());
}
void FWK::Graphics::DirectCommandList::SetupPipelineState(const PipelineState& a_pipelineState) const
{
	const auto& l_directCommandList = GetCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、パイプラインステートの設定が出来ませんでした。");
		return;
	}

	const auto& l_pipelineState = a_pipelineState.GetPipelineState();

	if (!l_pipelineState)
	{
		assert(false && "パイプラインステートが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

	// コマンドリストにパイプラインステートをセット
	l_directCommandList->SetPipelineState(l_pipelineState.Get());
}

void FWK::Graphics::DirectCommandList::DispatchMesh(const UINT a_groupX, const UINT a_groupY, const UINT a_groupZ) const
{
	const auto& l_directCommandList = GetCommandList().Get();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、DispatchMesh処理が出来ませんでした。");
		return;
	}

	l_directCommandList->DispatchMesh(a_groupX, a_groupY, a_groupZ);
}