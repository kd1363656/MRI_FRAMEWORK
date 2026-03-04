#include "RenderArea.h"

void FWK::Graphics::RenderArea::Init()
{
	m_viewport    = D3D12_VIEWPORT();
	m_scissorRect = D3D12_RECT    ();
}
bool FWK::Graphics::RenderArea::SetupRenderArea(const SwapChain& a_swapChain)
{
	const auto& l_swapChain = a_swapChain.GetSwapChain();

	if (!l_swapChain)
	{
		assert(false && "スワップチェインが作成されておらず、ビューポート及びシザーレクトの作成に失敗しました。");
		return false;
	}

	const auto& l_backBufferList = a_swapChain.GetBackBufferList();
	
	if (l_backBufferList.empty())
	{
		assert(false && "バックバッファーの要素が空になっており、ビューポート及びシザーレクトの作成に失敗しました。");
		return false;
	}

	const auto& l_firstBackBuffer = l_backBufferList.begin()->Get();

	if (l_backBufferList.empty())
	{
		assert(false && "バックバッファー作成ができておらず、ビューポート及びシザーレクトの作成に失敗しました。");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC1 l_desc = {};

	// スワップチェインからパラメータを取得し縦幅と横幅を取得
	auto l_hr = l_swapChain->GetDesc1(&l_desc);
	
	if (FAILED(l_hr))
	{
		assert(false && "スワップチェインのパラメーターの取得に失敗しており、ビューポート及びシザーレクトの作成に失敗しました。");
		return false;
	}

	// "ViewPort : NDC(-1 ~ 1) ->" 画面ピクセルへの変換領域
	m_viewport = CD3DX12_VIEWPORT(l_firstBackBuffer);

	// "ScissorRect"は描画してよいピクセル範囲(今回は全画面)
	m_scissorRect = CD3DX12_RECT(k_defaultScissorRectLeft, 
								 k_defaultScissorRectTop, 
								 static_cast<LONG>(l_desc.Width),
								 static_cast<LONG>(l_desc.Height));

	return true;
}