#include "Renderer.h"

FWK::Render::Renderer::Renderer() : 
	m_hardware                (),
	m_rendererJsonConverter   (*this),
	m_syncInterval            (CommonConstant::k_defaultSyncInterval)
{}
FWK::Render::Renderer::~Renderer() = default;

void FWK::Render::Renderer::Init()
{
	m_hardware.Init();
}
bool FWK::Render::Renderer::Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!m_hardware.Create())
	{
		assert(false && "レンダーデバイスの作成に失敗しました。");
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
}
void FWK::Render::Renderer::SaveCONFIG()
{
	auto l_rootJson = m_rendererJsonConverter.Serialize();

	l_rootJson["Hardware"] = m_hardware.Serialize ();
	
	Utility::FileIO::SaveJsonFile(l_rootJson, k_configFileIOPath);
}