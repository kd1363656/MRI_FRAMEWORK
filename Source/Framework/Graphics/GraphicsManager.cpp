#include "GraphicsManager.h"

void FWK::Graphics::GraphicsManager::Init()
{
#if defined(_DEBUG)
	EnableDebugLayer();
#endif

	m_hardware.Init();
}
bool FWK::Graphics::GraphicsManager::Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!m_hardware.Create())
	{
		assert(false && "ハードウェアの作成処理に失敗しました。");
		return false;
	}

	return true;
}
void FWK::Graphics::GraphicsManager::PostCreateSetup(const HWND& a_hWND)
{

}

void FWK::Graphics::GraphicsManager::LoadConfig()
{
	const auto& l_json = Utility::File::LoadJsonFile(k_configFileIOPath);

	if (l_json.is_null()) { return; }
}
void FWK::Graphics::GraphicsManager::SaveConfig()
{
	auto l_json = nlohmann::json();

	Utility::File::SaveJsonFile(l_json, k_configFileIOPath);
}

void FWK::Graphics::GraphicsManager::BeginDraw()
{

}
void FWK::Graphics::GraphicsManager::Draw()
{

}
void FWK::Graphics::GraphicsManager::EndDraw()
{
}

#if defined(_DEBUG)
bool FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	Microsoft::WRL::ComPtr<ID3D12Debug5> l_debug = nullptr;

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