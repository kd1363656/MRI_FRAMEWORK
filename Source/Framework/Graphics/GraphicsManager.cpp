#include "GraphicsManager.h"

void FWK::Graphics::GraphicsManager::Init() const
{
#if defined(_DEBUG)
	if (!EnableDebugLayer())
	{
		assert(false && "デバッグレイヤーの有効化に失敗しました。");
		return;
	}
#endif
}
void FWK::Graphics::GraphicsManager::LoadCONFIG()
{
	const auto& l_rootJson = Utility::File::LoadJsonFile(k_configFileIOPath);

	if (l_rootJson.is_null()) { return; }

	m_graphicsManagerJsonConverter.Deserialize(l_rootJson, *this);
}
bool FWK::Graphics::GraphicsManager::Create(const HWND& a_hwnd, const Struct::WindowCONFIG& a_windowCONFIG)
{
	if (!m_factory.Create())
	{
		assert(false && "ファクトリーの作成に失敗しました。");
		return false;
	}

	if (!m_device.Create(m_factory))
	{
		assert(false && "デバイスの作成処理に失敗しました。");
		return false;
	}

	if (!m_resourceContext.Create(m_device))
	{
		assert(false && "リソースコンテキストの作成処理に失敗思案した。");
		return false;
	}

	if (!m_renderer.Create(m_device))
	{
		assert(false && "レンダラーの作成処理に失敗しました。");
		return false;
	}

	if (!m_swapChain.Create(a_hwnd,
							m_device,
						    m_factory,
						    m_renderer.GetREFDirectCommandQueue(),
							a_windowCONFIG,
							m_resourceContext.GetMutableREFRTVDescriptorPool()))
	{
		assert(false && "スワップチェインの作成処理に失敗しました。");
		return false;
	}

    return true;
}

void FWK::Graphics::GraphicsManager::PostCreateSetup(const HWND& a_hWND)
{
	m_swapChain.PostCreateSetup(a_hWND, m_factory);
	m_renderer.PostCreateSetup (m_swapChain);
}

void FWK::Graphics::GraphicsManager::BeginDraw()
{
	const auto& l_rtvDescriptorHeap = m_resourceContext.GetREFRTVDescriptorPool().GetREFDescriptorHeap();

	m_renderer.BeginDraw(m_swapChain, l_rtvDescriptorHeap);
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

#if defined(_DEBUG)
bool FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	TypeAlias::ComPtr<ID3D12Debug5> l_debug = nullptr;

	// デバッグ機能を有効化するためのインターフェースを取得する関数
	// D3D12GetDebugInterface(受け取りたいCOMインターフェース型のID、
	//					      作成結果のポインタを書き込むアドレス);

	auto l_hr = D3D12GetDebugInterface(IID_PPV_ARGS(l_debug.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "デバッグレイヤーの有効化に失敗しました。");
		return false;
	}

	l_debug->EnableDebugLayer();

	return true;
}
#endif