#include "GraphicsManager.h"

FWK::Graphics::GraphicsManager::GraphicsManager() : 
	m_hardware()
{}
FWK::Graphics::GraphicsManager::~GraphicsManager() = default;

void FWK::Graphics::GraphicsManager::Init()
{
	m_hardware.Init();
}
bool FWK::Graphics::GraphicsManager::Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!m_hardware.Create())
	{
		assert(false && "ハードウェアの作成に失敗。");
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
}
void FWK::Graphics::GraphicsManager::SaveConfig()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["Hardware"] = m_hardware.Serialize();

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