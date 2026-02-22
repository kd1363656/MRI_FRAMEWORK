#include "Renderer.h"

FWK::Render::Renderer::Renderer() : 
	m_frameResourceList     (),
	m_hardware              (),
	m_graphicsCommandContext(m_hardware),
	m_swapChain             (m_hardware, m_graphicsCommandContext.GetGraphicsCommandQueue()),
	m_fence                 (m_hardware, m_graphicsCommandContext.GetGraphicsCommandQueue()),
	m_rendererJsonConverter (*this),
	m_syncInterval          (CommonConstant::k_defaultSyncInterval)
{}
FWK::Render::Renderer::~Renderer() = default;

void FWK::Render::Renderer::Init()
{
	m_frameResourceList.clear();

	m_hardware.Init				 ();
	m_graphicsCommandContext.Init();
	m_swapChain.Init             ();
	m_fence.Init                 ();

	m_syncInterval = CommonConstant::k_defaultSyncInterval;
}
bool FWK::Render::Renderer::Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!m_hardware.Create())
	{
		assert(false && "レンダーデバイスの作成に失敗しました。");
		return false;
	}

	if (!m_graphicsCommandContext.Create())
	{
		assert(false && "コマンドオブジェクト(ID3D12_COMMAND_TYPE_DIRECT)の作成に失敗しました。");
		return false;
	}

	if (!m_swapChain.Create(a_hWND, a_windowConfig))
	{
		assert(false && "スワップチェインの作成に失敗しました。");
		return false;
	}

	// バックバッファの数を取得してその数に対応したフレームリソースを作成
	auto l_backBufferNum = m_swapChain.GetBackBufferNum();

	// 配列の要素数を予約してから追加
	m_frameResourceList.reserve(l_backBufferNum);
	for (UINT l_i = 0U; l_i < l_backBufferNum; ++l_i)
	{
		m_frameResourceList.emplace_back(m_graphicsCommandContext);
	}

	for (UINT l_i = 0U; l_i < l_backBufferNum; ++l_i)
	{
		// 一応初期化
		m_frameResourceList[l_i].Init();

		if (!m_frameResourceList[l_i].Create())
		{
			assert(false && "フレームリソースの作成に失敗。");
			return false;
		}
	}

	if (!m_fence.Create())
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	return true;
}
void FWK::Render::Renderer::PostCreateInit(const HWND& a_hWND)
{
	// Alt + Enterで勝手に排他制御フルスクリーンにされるのを防ぐ処理
	m_hardware.PostCreateInit(a_hWND);

	// 現フレームで使用しようとしているコマンドリストのリセットを行う(しないとエラーが発生するため)
	m_graphicsCommandContext.ResetCommandObjects(m_frameResourceList, m_swapChain);
}

void FWK::Render::Renderer::LoadCONFIG()
{
	const auto& l_rootJson = Utility::FileIO::LoadJsonFile(k_configFileIOPath);
	
	if (l_rootJson.is_null()) { return; }

	m_rendererJsonConverter.Deserialize(l_rootJson);

	if (l_rootJson.contains("Hardware"))
	{
		m_hardware.Deserialize(l_rootJson["Hardware"]);
	}

	if (l_rootJson.contains("SwapChain"))
	{
		m_swapChain.Deserialize(l_rootJson["SwapChain"]);
	}
}
void FWK::Render::Renderer::SaveCONFIG()
{
	auto l_rootJson = m_rendererJsonConverter.Serialize();

	l_rootJson["Hardware"]  = m_hardware.Serialize ();
	l_rootJson["SwapChain"] = m_swapChain.Serialize();

	Utility::FileIO::SaveJsonFile(l_rootJson, k_configFileIOPath);
}

void FWK::Render::Renderer::BeginFrame()
{
	const auto& l_graphicsCommandList    = m_graphicsCommandContext.GetGraphicsCommandList();
	const auto& l_backBufferList         = m_swapChain.GetBackBufferList				  ();
	const auto  l_currentBackBufferIndex = m_swapChain.GetCurrentBackBufferIndex          ();

	// 今から使用するコマンドアロケータが再利用可能かどうかをチェック
	// 再利用できる状態でない(コマンドアロケータを使ったGPU処理をしている最中なら同期をとる)
	m_fence.BeginFrame(m_frameResourceList, m_swapChain);

	// リソースの状態遷移("Present -> RenderTarget")
	l_graphicsCommandList.TransitionResource(l_backBufferList[l_currentBackBufferIndex].Get(), 
										     D3D12_RESOURCE_STATE_PRESENT,
											 D3D12_RESOURCE_STATE_RENDER_TARGET,
										     k_sendRenderTargetBarrierStateNum);

	// 描画命令を出すためのレンダーターゲットを指定し、所定のカラーでレンダーターゲットをクリアしている
	l_graphicsCommandList.SetUpRenderTarget(m_swapChain, k_executeRenderTargetNum);
}

void FWK::Render::Renderer::EndFrame()
{
	const auto& l_graphicsCommandList    = m_graphicsCommandContext.GetGraphicsCommandList();
	const auto& l_backBufferList         = m_swapChain.GetBackBufferList				  ();
	const auto  l_currentBackBufferIndex = m_swapChain.GetCurrentBackBufferIndex          ();

	// リソースの状態遷移("RenderTarget -> Present")
	l_graphicsCommandList.TransitionResource(l_backBufferList[l_currentBackBufferIndex].Get(), 
											 D3D12_RESOURCE_STATE_RENDER_TARGET,
										     D3D12_RESOURCE_STATE_PRESENT,
										     k_sendRenderTargetBarrierStateNum);

	// GPUに描画命令を出す
	m_graphicsCommandContext.EndFrame(m_fence, k_executeCommandListNum);

	// フェンス地を更新
	m_fence.EndFrame(m_frameResourceList, m_swapChain, m_graphicsCommandContext.GetGraphicsCommandQueue());

	// リセット処理
	m_graphicsCommandContext.ResetCommandObjects(m_frameResourceList, m_swapChain);

	// 設定したバックバッファを画面に描画
	m_swapChain.Present(m_syncInterval);
}