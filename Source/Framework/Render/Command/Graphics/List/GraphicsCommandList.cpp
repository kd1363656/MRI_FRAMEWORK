#include "GraphicsCommandList.h"

FWK::Render::GraphicsCommandList::GraphicsCommandList(const GraphicsCommandContext& a_graphicsCommandContext) : 
	CommandListBase         (a_graphicsCommandContext.GetHardware(), CommonConstant::k_createCommandListTypeDirect),
	k_graphicsCommandContext(a_graphicsCommandContext)
{}
FWK::Render::GraphicsCommandList::~GraphicsCommandList() = default;

void FWK::Render::GraphicsCommandList::Reset(const TypeDEF::ComPtr<ID3D12CommandAllocator>& a_commandAllocator) const
{
	if (!a_commandAllocator)
	{
		assert(false && "コマンドアロケーターが作成されておらず、コマンドリストのリセットが出来ませんでした。");
		return;
	}

	const auto& l_commandList = GetCommandList().Get();

	if (!l_commandList)
	{
		assert(false && "グラフィックスコマンドリストが作成されておらず、コマンドリストのリセットが出来ませんでした。");
		return;
	}

	l_commandList->Reset(a_commandAllocator.Get(), nullptr);
}

void FWK::Render::GraphicsCommandList::TransitionResource(const TypeDEF::ComPtr<ID3D12Resource2>& a_resource, 
														  const D3D12_RESOURCE_STATES             a_beforState,
														  const D3D12_RESOURCE_STATES			  a_afterState,
														  const UINT							  a_sendResourceBarrierNum) const
{
	const auto& l_graphicsCommandList = GetCommandList().Get();

	if (!l_graphicsCommandList)
	{
		assert(false && "グラフィックスコマンドリストが作成されておらず、リソースの状態遷移が出来ませんでした。");
		return;
	}

	// リソースの状態遷移
	// a_beforState -> a_afterState
	const auto& l_barrier = CD3DX12_RESOURCE_BARRIER::Transition(a_resource.Get(), a_beforState, a_afterState);

	// リソースバリアを実行
	l_graphicsCommandList->ResourceBarrier(a_sendResourceBarrierNum, &l_barrier);
}

void FWK::Render::GraphicsCommandList::SetUpRenderTarget(const SwapChain& a_swapChain, const UINT a_executeRenderTargetNum) const
{
	const auto& l_graphicsCommandList = GetCommandList().Get();

	if (!l_graphicsCommandList)
	{
		assert(false && "グラフィックスコマンドリストが作成されておらず、リソースの状態遷移が出来ませんでした。");
		return;
	}

	const auto  l_currentBackBufferIndex = a_swapChain.GetCurrentBackBufferIndex();

	// バックバッファーの現在のインデックスに対応したハンドルを取得
	auto l_rtvH = a_swapChain.GetRTVHeap().GetCPUHandle(l_currentBackBufferIndex);

	// 描画命令を出すレンダーターゲットをセット
	l_graphicsCommandList->OMSetRenderTargets(a_executeRenderTargetNum,
											  &l_rtvH,
											  true,
											  nullptr);

	// 画面を所定の色でクリア
	l_graphicsCommandList->ClearRenderTargetView(l_rtvH,
											     k_clearColor, 
											     k_executeClearRectNum,
												 nullptr);
}

void FWK::Render::GraphicsCommandList::Close() const
{
	const auto& l_graphicsCommandList = GetCommandList().Get();

	if (!l_graphicsCommandList)
	{
		assert(false && "グラフィックスコマンドリストが作成されておらず、コマンドリストのリセットが出来ませんでした。");
		return;
	}

	l_graphicsCommandList->Close();
}