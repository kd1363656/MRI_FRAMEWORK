#include "GraphicsManager.h"

FWK::Graphics::GraphicsManager::GraphicsManager() : 
	m_hardware       (),
	m_renderer       (m_hardware),
	m_resourceContext(m_hardware.GetDevice()),
	m_swapChain      (m_hardware, m_resourceContext.GetDescriptorHeapContext().GetRTVDescriptroHeap())
{}
FWK::Graphics::GraphicsManager::~GraphicsManager() = default;

void FWK::Graphics::GraphicsManager::Init()
{
	m_hardware.Init       ();
	m_renderer.Init       ();
	m_resourceContext.Init();
	m_swapChain.Init      ();
}
bool FWK::Graphics::GraphicsManager::Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!m_hardware.Create())
	{
		assert(false && "ハードウェアの作成に失敗。");
		return false;
	}

	if (!m_renderer.Create())
	{
		assert(false && "レンダラーの作成に失敗。");
		return false;
	}

	if (!m_resourceContext.Create(m_swapChain))
	{
		assert(false && "リソースコンテキストの作成に失敗");
		return false;
	}

	if (!m_swapChain.Create(a_hWND, a_windowConfig, m_renderer.GetDeirectCommandQueue(), m_resourceContext.GetDescriptorHeapContext().GetRTVDescriptroHeap()))
	{
		assert(false && "スワップチェインの作成に失敗");
		return false;
	}

	return true;
}
void FWK::Graphics::GraphicsManager::PostLoadSetup(const HWND& a_hWND)
{
}

void FWK::Graphics::GraphicsManager::LoadConfig()
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
void FWK::Graphics::GraphicsManager::SaveConfig()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["Hardware"]  = m_hardware.Serialize ();
	l_rootJson["SwapChain"] = m_swapChain.Serialize();

	Utility::FileIO::SaveJsonFile(l_rootJson, k_configFileIOPath);
}

void FWK::Graphics::GraphicsManager::BeginFrame()
{
}
void FWK::Graphics::GraphicsManager::Draw()
{
}
void FWK::Graphics::GraphicsManager::EndFrame()
{
}