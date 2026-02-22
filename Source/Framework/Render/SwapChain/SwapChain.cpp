#include "SwapChain.h"

FWK::Render::SwapChain::SwapChain(const Hardware& a_hardware, const GraphicsCommandQueue& a_graphicsCommandQueue) : 
	k_hardware              (a_hardware),
	k_graphicsCommandQueue  (a_graphicsCommandQueue),
	m_backBufferList        (),
	m_swapChain			    (nullptr),
	m_rtvHeap               (a_hardware, CommonConstant::k_descriptorHeapTypeRTV),
	m_swapChainJsonConverter(*this),
	m_backBufferNum         (CommonConstant::k_defaultBackBufferNum)
{}
FWK::Render::SwapChain::~SwapChain() = default;

void FWK::Render::SwapChain::Init()
{
	m_backBufferList.clear();

	m_swapChain.Reset();

	m_backBufferNum = CommonConstant::k_defaultBackBufferNum;
}
bool FWK::Render::SwapChain::Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!CreateSwapChain(a_hWND, a_windowConfig))
	{
		assert(false && "スワップチェインの作成に失敗しました。");
		return false;
	}

	if (!CreateRTVHeap())
	{
		assert(false && "RTVHeapの作成に失敗しました。");
		return false;
	}

	if (!CreateBackBufferList())
	{
		assert(false && "バックバッファーリストの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Render::SwapChain::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_swapChainJsonConverter.Deserialize(a_rootJson);
}
nlohmann::json FWK::Render::SwapChain::Serialize()
{
	return m_swapChainJsonConverter.Serialize();
}

bool FWK::Render::SwapChain::CreateSwapChain(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	const auto& l_factory = k_hardware.GetFactory();

	if (!l_factory) 
	{
		assert(false && "ファクトリーの作成がされておらず、スワップチェインの作成が出来ません。");
		return false;
	}

	// TypeがDirect出なければreturn(作成できないから)
	if (k_graphicsCommandQueue.GetCreateCommandListType() != CommonConstant::k_createCommandListTypeDirect)
	{
		assert(false && "コマンドキューのコマンドリストタイプは描画可能なDirectでないといけません。");
		return false;
	}

	const auto& l_graphicsCommandQueue = k_graphicsCommandQueue.GetCommandQueue();

	if (!l_graphicsCommandQueue)
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
	TypeDEF::ComPtr<IDXGISwapChain1> l_swapChain = nullptr;

	// スワップチェイン作成
	auto l_hr = l_factory->CreateSwapChainForHwnd(l_graphicsCommandQueue.Get(), 
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
bool FWK::Render::SwapChain::CreateRTVHeap()
{
	if (m_backBufferNum == 0U)
	{
		assert(false && "作成するバックバッファの数が0でした、RTVヒープの作成ができません。");
		return false;
	}

	// "RTV"ヒープ作製("ShaderVisible"不要)
	if (!m_rtvHeap.Create(m_backBufferNum, false))
	{
		assert(false && "RTVヒープの作成に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Render::SwapChain::CreateBackBufferList()
{
	const auto& l_device = k_hardware.GetDevice();

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

		const auto& l_rtvHandle = m_rtvHeap.GetCPUHandle(l_i);

		// レンダーターゲットビューを作成
		l_device->CreateRenderTargetView(m_backBufferList[l_i].Get(), nullptr, l_rtvHandle);
	}

	return true;
}