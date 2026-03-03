#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList(const Device& a_device) : 
	CommandListBase(a_device, D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;

void FWK::Graphics::DirectCommandList::TransitionResource(const ComPtr<ID3D12Resource2>& a_resourece, const D3D12_RESOURCE_STATES a_beforState, const D3D12_RESOURCE_STATES a_afterState) const 
{
	const auto& l_commandList = GetCommandList();

	if (!a_resourece)
	{
		assert(false && "リソースが作成されておらず、リソースの繊維ができませんでした。");
		return;
	}

	if (!l_commandList)
	{
		assert(false && "コマンドリストが作成されておらず、リソースの状態遷移が出来ませんでした。");
		return;
	}

	// リソースの状態遷移
	// a_beforState -> a_afterState
	const auto& l_barrier = CD3DX12_RESOURCE_BARRIER::Transition(a_resourece.Get(), a_beforState, a_afterState);

	// リソースバリアを実行
	l_commandList->ResourceBarrier(k_sendBarrierNum, &l_barrier);
}

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