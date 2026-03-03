#include "SwapChain.h"

FWK::Graphics::SwapChain::SwapChain(const Hardware& a_hardware) : 
    k_hardware              (a_hardware),
	m_backBufferList        (),
	m_swapChain             (nullptr),
	m_swapChainJsonConverter(*this),
	m_backBufferNum         (CommonConstant::k_defaultBackBufferNum),
	m_syncInterval          (CommonConstant::k_defaultSyncInterval)
{}
FWK::Graphics::SwapChain::~SwapChain() = default;

void FWK::Graphics::SwapChain::Init()
{
	m_backBufferList.clear();

	m_swapChain.Reset();

	m_backBufferNum = CommonConstant::k_defaultBackBufferNum;
	m_syncInterval  = CommonConstant::k_defaultSyncInterval;
}

bool FWK::Graphics::SwapChain::Create(const HWND&                       a_hWND, 
                                      const CommonStruct::WindowConfig& a_windowConfig,
                                      const DirectCommandQueue&         a_directCommandQueue,
                                      const RTVDescriptorHeap&          a_rtvDescriptorHeap)
{
	if (!CreateSwapChain(a_hWND, a_windowConfig, a_directCommandQueue))
	{
		assert(false && "スワップチェインの作成に失敗しました。");
		return false;
	}

	if (!CreateBackBufferList(a_rtvDescriptorHeap))
	{
		assert(false && "バックバッファーリストの作成に失敗しました。");
		return false;
	}

	return true;
}
void FWK::Graphics::SwapChain::PostCreateSetup(const HWND& a_hWND) const
{
	const auto& l_factory = k_hardware.GetFactory().GetFactory();

	if (!l_factory)
	{
		assert(false && "ファクトリーの作成に失敗しており排他スクリーン設定ができませんでした。");
		return;
	}

	// Alt + Enterで勝手に排他制御フルスクリーンにされるのを防ぐ
	l_factory->MakeWindowAssociation(a_hWND, DXGI_MWA_NO_ALT_ENTER);
}

void FWK::Graphics::SwapChain::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_swapChainJsonConverter.Deserialize(a_rootJson);
}
nlohmann::json FWK::Graphics::SwapChain::Serialize()
{
	return m_swapChainJsonConverter.Serialize();
}

void FWK::Graphics::SwapChain::Present() const
{
	if (!m_swapChain)
	{
		assert(false && "スワップチェインが作成されておらず、現在のバックバッファのインデックスを取得出来ませんでした。");
		return;
	}

	m_swapChain->Present(m_syncInterval, k_swapChainPresentFlagNone);
}

UINT FWK::Graphics::SwapChain::GetCurrentBackBufferIndex() const
{
	if (!m_swapChain)
	{
		assert(false && "スワップチェインが作成されておらず、現在のバックバッファのインデックスを取得出来ませんでした。");
		return 0U;
	}

	return m_swapChain->GetCurrentBackBufferIndex();
}

bool FWK::Graphics::SwapChain::CreateSwapChain(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig, const DirectCommandQueue& a_directCommandQueue)
{
	const auto& l_factory = k_hardware.GetFactory().GetFactory();

	if (!l_factory) 
	{
		assert(false && "ファクトリーの作成がされておらず、スワップチェインの作成が出来ません。");
		return false;
	}

	// TypeがDirect出なければreturn(作成できないから)
	if (a_directCommandQueue.GetCreateCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		assert(false && "コマンドキューのコマンドリストタイプは描画可能なDirectでないといけません。");
		return false;
	}

	const auto& l_commandQueue = a_directCommandQueue.GetCommandQueue();

	if (!l_commandQueue)
	{
		assert(false && "コマンドキューの作成がされていないためスワップチェインが作成できません。");
		return false;
	}

	if (m_backBufferNum == 0U)
	{
		assert(false && "作成するバックバッファの数が0でした、スワップチェインの作成ができません。");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC1 l_desc = {};

	l_desc.Width              = a_windowConfig.width;            // 0を指定するとウィンドウサイズに合わせてくれる、ウィンドウのクライアントサイズの幅
	l_desc.Height             = a_windowConfig.height;           // 0を指定するとウィンドウサイズに合わせてくれる、ウィンドウのクライアントサイズの高さ
	l_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;      // 1ピクセルをどういうビット構造で保存するか(バックバッファのピクセル形式)
	l_desc.Stereo			  = FALSE;					         // 立体視野フラグ
	l_desc.SampleDesc.Count   = k_defaultSampleCount;	         // ピクセル当たりのマルチサンプル数
	l_desc.SampleDesc.Quality = k_defaultSampleQuality;          // 品質レベル(最低)
	l_desc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バックバッファーをレンダーターゲットとして扱うための指定
	l_desc.BufferCount        = m_backBufferNum;				 // スワップチェイン内のバックバッファの枚数、基本は2個でよい
	l_desc.Scaling            = DXGI_SCALING_STRETCH;			 // ウィンドウサイズ変更時のスケーリング方式
	l_desc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;	 // 表示方式(フリップ後は早やかに破棄)
	l_desc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;	 // 合成(透過)に関する設定。普通のウィンドウなら"UNSPECIFIED"
	l_desc.Flags              = 0U;								 // オプションフラグ

	// CreateSwapChainForHwndが引数で要求する型がIDXGISwapChain1なので型を合わせる
	ComPtr<IDXGISwapChain1> l_swapChain = nullptr;

	// スワップチェイン作成
	auto l_hr = l_factory->CreateSwapChainForHwnd(l_commandQueue.Get(), 
												  a_hWND, 
												  &l_desc, 
												  nullptr,
												  nullptr,
												  l_swapChain.ReleaseAndGetAddressOf());

	if (FAILED(l_hr))
	{
		assert(false && "スワップチェインの作成に失敗しました。");
		return false;
	}

	// As関数はクエリインターフェースを使って安全に型変換を行う
	l_hr = l_swapChain.As(&m_swapChain);
	if (FAILED(l_hr) || !m_swapChain)
	{
		assert(false && "スワップチェインの型変換に失敗しました、スワップチェインの作製が行えません。");
		return false;
	}

	return true;
}
bool FWK::Graphics::SwapChain::CreateBackBufferList(const RTVDescriptorHeap& a_rtvDescriptorHeap)
{
	const auto& l_device = k_hardware.GetDevice().GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作成が行われていないためバックバッファーを作成できません。");
		return false;
	}

	if (!m_swapChain)
	{
		assert(false && "スワップチェインの作成が行われていないためバックバッファーを作成できません。");
		return false;
	}

	// バックバッファ数分要素を確保
	m_backBufferList.resize(m_backBufferNum);

	// 念のため要素を全てReset
	for (auto& l_backBuffer : m_backBufferList)
	{
		l_backBuffer.Reset();
	}
	
	// SRGB用レンダーターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC l_rtvDesc = {};

	l_rtvDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
	l_rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// BackBuffer取得 + RTV作製
	for (std::uint32_t l_i = 0U; l_i < m_backBufferNum; ++l_i)
	{
		// スワップチェインとバッファーの紐づけ
		const auto l_hr = m_swapChain->GetBuffer(l_i, IID_PPV_ARGS(m_backBufferList[l_i].ReleaseAndGetAddressOf()));

		if (FAILED(l_hr))
		{
			assert(false && "スワップチェインとバックバッファーの紐づけに失敗しました。");
			return false;
		}

		// ディスクリプタを示すハンドルを取得
		const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchCPUHandle(l_i);

		// レンダーターゲットビューを作成
		l_device->CreateRenderTargetView(m_backBufferList[l_i].Get(), &l_rtvDesc, l_rtvHandle);
	}

	return true;
}