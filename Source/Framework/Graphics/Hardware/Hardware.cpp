#include "Hardware.h"

FWK::Graphics::Hardware::Hardware() : 
	m_factory(),
	m_device (m_factory)
{}
FWK::Graphics::Hardware::~Hardware() = default;

void FWK::Graphics::Hardware::Init()
{
#if defined(_DEBUG)
	if (!EnableDebugLayer())
	{
		assert(false && "デバックレイヤーの有効化に失敗しました。");
		return;
	}
#endif

	m_factory.Init();
	m_device.Init ();
}
bool FWK::Graphics::Hardware::Create()
{
	if (!m_factory.Create())
	{
		assert(false && "ファクトリーの作成に失敗しました。");
		return false;
	}

	if (!m_device.Create())
	{
		assert(false && "デバイスの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::Hardware::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	
	if (a_rootJson.contains("Device"))
	{
		m_device.Deserialize(a_rootJson["Device"]);
	}
}
nlohmann::json FWK::Graphics::Hardware::Serialize()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["Device"] = m_device.Serialize();

	return l_rootJson;
}

#if defined(_DEBUG)
bool FWK::Graphics::Hardware::EnableDebugLayer() const
{
	ComPtr<ID3D12Debug5> l_debug = nullptr;

	// デバッグ機能を有効化するためのインターフェースを取得
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