#include "Renderer.h"

FWK::Render::Renderer::Renderer() : 
	m_hardware              (),
	m_graphicsCommandContext(m_hardware),
	m_swapChain             (m_hardware, m_graphicsCommandContext.GetGraphicsCommandQueue()),
	m_rendererJsonConverter (*this),
	m_syncInterval          (CommonConstant::k_defaultSyncInterval)
{}
FWK::Render::Renderer::~Renderer() = default;

void FWK::Render::Renderer::Init()
{
	m_hardware.Init				 ();
	m_graphicsCommandContext.Init();
	m_swapChain.Init             ();

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

	return true;
}
void FWK::Render::Renderer::PostCreateInit(const HWND& a_hWND)
{
	// Alt + Enterで勝手に排他制御フルスクリーンにされるのを防ぐ処理
	m_hardware.PostCreateInit(a_hWND);
}

void FWK::Render::Renderer::LoadCONFIG()
{
	const auto& l_rootJson = Utility::FileIO::LoadJsonFile(k_configFileIOPath);
	
	if (l_rootJson.is_null()) { return; }

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