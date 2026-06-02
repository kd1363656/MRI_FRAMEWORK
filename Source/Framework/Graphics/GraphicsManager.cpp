#include "GraphicsManager.h"

void FWK::Graphics::GraphicsManager::INIT()
{
	m_renderer.INIT();

#if defined(_DEBUG)
	FWK_ASSERT_RETURN_IF(!EnableDebugLayer(), "デバッグレイヤーの有効化に失敗しました。")
#endif
}
void FWK::Graphics::GraphicsManager::LoadCONFIG()
{
	const auto& l_rootJson = Utility::File::LoadJsonFile(k_configFileIOPath);

	if (l_rootJson.is_null()) { return; }

	m_graphicsManagerJsonConverter.Deserialize(l_rootJson, *this);
}
void FWK::Graphics::GraphicsManager::PostDeserializeSetup(const Struct::WindowCONFIG& a_windowCONFIG)
{
	m_renderer.PostDeserializeSetup(a_windowCONFIG);
}
bool FWK::Graphics::GraphicsManager::Create(const HWND& a_hwnd, const Struct::WindowCONFIG& a_windowCONFIG)
{
	FWK_ASSERT_RETURN_VALUE_IF(!m_factory.Create(),					"ファクトリーの作成に失敗しました。",			  false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_device.Create(m_factory),			"デバイスの作成処理に失敗しました。",			  false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_resourceContext.Create(m_device), "リソースコンテキストの作成処理に失敗しました。", false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_shaderCompiler.Create(),		    "シェーダーコンパイラの作成処理に失敗しました。", false)


	FWK_ASSERT_RETURN_VALUE_IF(!m_renderer.Create(m_device, 
							   m_shaderCompiler,
							   m_resourceContext.GetREFGPUMemoryAllocator(),
							   a_windowCONFIG.m_clientSize.m_width,
							   a_windowCONFIG.m_clientSize.m_height,
							   m_resourceContext.GetMutableREFRTVDescriptorPool(),
							   m_resourceContext.GetMutableREFSRVDescriptorPool(),
							   m_resourceContext.GetMutableREFDSVDescriptorPool()),
							   "レンダラーの作成処理に失敗しました。",
							   false)

	FWK_ASSERT_RETURN_VALUE_IF(!m_swapChain.Create(a_hwnd,
												   m_device,
												   m_factory,
												   m_renderer.GetREFDirectCommandQueue(),
												   a_windowCONFIG,
												   m_resourceContext.GetMutableREFRTVDescriptorPool()),
												   "スワップチェインの作成処理に失敗しました。",
												   false)

    return true;
}
void FWK::Graphics::GraphicsManager::PostCreateSetup(const HWND& a_hwnd)
{
	m_swapChain.PostCreateSetup(a_hwnd, m_factory);
	m_renderer.PostCreateSetup (m_swapChain);
}

void FWK::Graphics::GraphicsManager::BeginFrame()
{
	m_resourceContext.BeginFrame(m_renderer.GetREFDirectCommandQueue());
	m_renderer.BeginFrame       ();
}

void FWK::Graphics::GraphicsManager::BeginDraw()
{
	m_renderer.BeginDraw();
}
void FWK::Graphics::GraphicsManager::Draw()
{
	const auto& l_srvDescriptorPool = m_resourceContext.GetREFSRVDescriptorPool();
	const auto& l_rtvDescriptorHeap = m_resourceContext.GetREFRTVDescriptorPool().GetREFDescriptorHeap();
	const auto& l_dsvDescriptorHeap = m_resourceContext.GetREFDSVDescriptorPool().GetREFDescriptorHeap();
	const auto& l_textureSystem     = m_resourceContext.GetREFTextureSystem    ();

	m_renderer.ExecuteRenderGraph(l_srvDescriptorPool,
								  l_rtvDescriptorHeap,
								  l_dsvDescriptorHeap,
								  m_swapChain,
								  l_textureSystem);
}
void FWK::Graphics::GraphicsManager::EndDraw()
{
	m_renderer.EndDraw(m_swapChain);
}
void FWK::Graphics::GraphicsManager::EndFrame()
{
	m_renderer.EndFrame();
}

void FWK::Graphics::GraphicsManager::SaveCONFIG() const
{
	const auto& l_rootJson = m_graphicsManagerJsonConverter.Serialize(*this);

	Utility::File::SaveJsonFile(l_rootJson, k_configFileIOPath);
}

bool FWK::Graphics::GraphicsManager::ApplyWindowResizeRequest(const Struct::WindowResizeRequest& a_resizeRequest)
{
	// window側でサイズ変更が起きていない場合は、何もしない
	if (!a_resizeRequest.m_isRequested) { return true; }

	// 最小化中はクライアント領域が0になることがある
	// この状態でSwapChainやRenderTargetを作り直すと、0サイズのGPUリソース作成になって失敗してしまう。
	if (a_resizeRequest.m_isMinimized) { return true; }

	// ResizeBuffers()の前に、Rendererが待つDirectCommandList側のBackBuffer参照を外す、
	// 前フレームのResourceBarrierなどがコマンドリスト内部に残っていると、
	// SwapChain::ReleaseBackBufferList()でComPtrをResetしてもResizeBuffers()が失敗することがある
	FWK_ASSERT_RETURN_VALUE_IF(!m_renderer.PrepareForSwapChainResize(),																			"スワップチェインリサイズ前処理に失敗しました。", false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_swapChain.Resize(m_device, a_resizeRequest.m_clientSize, m_resourceContext.GetMutableREFRTVDescriptorPool()), "スワップチェインのリサイズに失敗しました。",	  false)

	// SwapChainのBackBufferサイズが変わったため、
	// BackBufferを基準にしているViewportとScissorRectも作り直す。
	FWK_ASSERT_RETURN_VALUE_IF(!m_renderer.GetMutableREFRenderArea().SetupRenderArea(m_swapChain), "リサイズ後のビューポート及びシザー矩形の再作成に失敗しました。", false)
	
	const auto& l_retiredFenceValue = m_renderer.GetMutableREFDirectCommandQueue().FetchREFLastSignaledFenceValue();

	FWK_ASSERT_RETURN_VALUE_IF(!m_renderer.Resize(m_device,
												  m_resourceContext.GetREFGPUMemoryAllocator(),
												  a_resizeRequest.m_clientSize,
												  l_retiredFenceValue,
												  m_resourceContext.GetMutableREFRTVDescriptorPool(),
												  m_resourceContext.GetMutableREFSRVDescriptorPool(),
												  m_resourceContext.GetMutableREFDSVDescriptorPool(),
												  m_resourceContext.GetMutableREFDeferredResourceReleaseQueue()),
												  "Renderer管理リソースのリサイズに失敗しました。",
												  false)

	return true;
}

#if defined(_DEBUG)
bool FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	TypeAlias::ComPtr<ID3D12Debug5> l_debug = nullptr;

	// デバッグ機能を有効化するためのインターフェースを取得する関数
	// D3D12GetDebugInterface(受け取りたいCOMインターフェース型のID、
	//					      作成結果のポインタを書き込むアドレス);
	auto l_hr = D3D12GetDebugInterface(IID_PPV_ARGS(l_debug.ReleaseAndGetAddressOf()));

	FWK_ASSERT_RETURN_VALUE_IF(FAILED(l_hr), "デバッグレイヤーの有効化に失敗しました。", false)

	l_debug->EnableDebugLayer();

	return true;
}
#endif