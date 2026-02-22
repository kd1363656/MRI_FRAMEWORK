#include "GraphicsCommandContext.h"

FWK::Render::GraphicsCommandContext::GraphicsCommandContext(const Hardware& a_hardware) :
	k_hardware            (a_hardware),
	m_graphicsCommandQueue(*this),
	m_graphicsCommandList (*this)
{}
FWK::Render::GraphicsCommandContext::~GraphicsCommandContext() = default;

void FWK::Render::GraphicsCommandContext::Init()
{
	m_graphicsCommandQueue.Init();
	m_graphicsCommandList.Init ();
}
bool FWK::Render::GraphicsCommandContext::Create()
{
	if (!m_graphicsCommandQueue.Create())
	{
		assert(false && "グラフィックスコマンドキューの作成に失敗しました。");
		return false;
	}

	if (!m_graphicsCommandList.Create())
	{
		assert(false && "グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Render::GraphicsCommandContext::ResetCommandObjects(const std::vector<FrameResource>& a_frameResourceList, const SwapChain& a_swapChain) const
{
	// 現在のバックバッファのインデックスを取得
	const UINT l_currentBackBufferIndex = a_swapChain.GetCurrentBackBufferIndex();

	// コマンドアロケータリストよりも現在取得したバックバッファの数が多ければ"assert"
	if (a_frameResourceList.size() <= l_currentBackBufferIndex)
	{
		assert(false && "フレームリソースの数よりも取得したバックバッファのインデックスが容量を超えています。");
		return;
	}

	const auto& l_graphicsCommandAllocator = a_frameResourceList[l_currentBackBufferIndex].GetGraphicsCommandAllocator();

	l_graphicsCommandAllocator.Reset();
	m_graphicsCommandList.Reset     (l_graphicsCommandAllocator.GetCommandAllocator());
}

void FWK::Render::GraphicsCommandContext::EndFrame(const Fence& a_fence, const UINT a_executeCommandListNum) const
{
	const auto& l_fence                = a_fence.GetFence                      ().Get();
	const auto& l_graphicsCommandQueue = m_graphicsCommandQueue.GetCommandQueue();
	const auto& l_graphicsCommandList  = m_graphicsCommandList.GetCommandList  ().Get();

	if (!l_fence)
	{
		assert(false && "フェンスが作成されておらず、描画処理が行えませんでした");
		return;
	}

	if (!l_graphicsCommandQueue)
	{
		assert(false && "グラフィックスコマンドキューが作成されておらず、描画処理が行えませんでした");
		return;
	}

	if (!l_graphicsCommandList)
	{
		assert(false && "グラフィックスコマンドリストが作成されておらず、描画処理が行えませんでした");
		return;
	}

	// Closeで命令受付をやめてコマンドリストを実行
	m_graphicsCommandList.Close();

	ID3D12CommandList* l_list[] = { l_graphicsCommandList };

	l_graphicsCommandQueue->ExecuteCommandLists(a_executeCommandListNum, l_list);
}