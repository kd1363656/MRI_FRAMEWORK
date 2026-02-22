#include "Hardware.h"

FWK::Render::Hardware::Hardware() : 
	m_factory              (nullptr),
	m_device               (nullptr),
	m_hardwareJsonConverter(*this),
	m_gpuNodeMask          (CommonConstant::k_defaultGPUNodeMask)
{}
FWK::Render::Hardware::~Hardware() = default;

void FWK::Render::Hardware::Init()
{
	m_factory.Reset();
	m_device.Reset ();

	m_gpuNodeMask = CommonConstant::k_defaultGPUNodeMask;
}
bool FWK::Render::Hardware::Create()
{
#if defined(_DEBUG)
	if (!EnableDebugLayer())
	{
		assert(false && "デバックレイヤーの有効化に失敗しました。");
		return false;
	}
#endif

	if (!CreateFactory())
	{
		assert(false && "ファクトリーの作成に失敗しました。");
		return false;
	}

	if (!CreateDevice())
	{
		assert(false && "デバイスの作成に失敗しました。");
		return false;
	}

	return true;
}
void FWK::Render::Hardware::PostCreateInit(const HWND& a_hWND) const
{
	if (!m_factory)
	{
		assert(false && "ファクトリーの作成に失敗しており排他スクリーン設定ができませんでした。");
		return;
	}

	// Alt + Enterで勝手に排他制御フルスクリーンにされるのを防ぐ
	m_factory->MakeWindowAssociation(a_hWND, DXGI_MWA_NO_ALT_ENTER);
}

void FWK::Render::Hardware::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_hardwareJsonConverter.Deserialize(a_rootJson);
}
nlohmann::json FWK::Render::Hardware::Serialize()
{
	return m_hardwareJsonConverter.Serialize();
}

bool FWK::Render::Hardware::CreateFactory()
{
	UINT l_flags = 0U;

#if defined(_DEBUG)
	// デバックソリューションならデバックフラグを立てる
	l_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT hr = CreateDXGIFactory2(l_flags, IID_PPV_ARGS(m_factory.ReleaseAndGetAddressOf()));

	if (FAILED(hr))
	{
		assert(false && "ファクトリーの作成に失敗しました");
		return false;
	}

	return true;
}
bool FWK::Render::Hardware::CreateDevice()
{
	// ファクトリーが作成できていなければグラフィックスカードの列挙ができないためreturn
	if (!m_factory)
	{
		assert(false && "ファクトリーが作成がされておらず、デバイスの作成が出来ませんでした。");
		return false;
	}

	TypeDEF::ComPtr<IDXGIAdapter4> l_adapter         = nullptr;
	TypeDEF::ComPtr<IDXGIAdapter4> l_selectedAdapter = nullptr;

	D3D_FEATURE_LEVEL l_selectedFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// グラフィックスカードのフィーチャーレベルを高い順で列挙
	const std::vector<D3D_FEATURE_LEVEL> l_featureLevels =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	bool l_isFound = false;

	// GPUの優先度を指定してアダプターを列挙し高性能の"GPU"優先で取得
	for (UINT l_i = 0U; m_factory->EnumAdapterByGpuPreference(l_i,
															  DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
															  IID_PPV_ARGS(l_adapter.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND;
		++l_i)
	{
		DXGI_ADAPTER_DESC3 l_desc = {};

		// アダプターがnullptrなら処理を飛ばす
		if (!l_adapter) { continue; }

		// GPUアダプターのパラメーターを取得
		l_adapter->GetDesc3(&l_desc);

		// ソフトウェアアダプタは除外
		if (l_desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) { continue; }

		// 上位フィーチャーレベルからテスト
		for (auto l_level : l_featureLevels)
		{
			// デバイス作成ができるかどうかのチェック
			auto l_hr = D3D12CreateDevice(l_adapter.Get(), l_level, __uuidof(ID3D12Device9), nullptr);
			if (FAILED(l_hr)) { continue; }

			// チェックが通ったアダプターとフィーチャーレベルを格納
			l_selectedAdapter      = l_adapter;
			l_selectedFeatureLevel = l_level;
			l_isFound              = true;
			break;
		}

		// 見つかったらこれ以上for分で探す必要がないのでbreak
		if (l_isFound) { break; }
	}

	if (!l_isFound)
	{
		assert(false && "対応する\"GPU\"が見つかっておらず、デバイスの作成に失敗しました。");
		return false;
	}

	// テスト済みのアダプターとフィーチャーレベルでデバイスを生成
	HRESULT l_hr = D3D12CreateDevice(l_selectedAdapter.Get(), l_selectedFeatureLevel, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "デバイスの作成に失敗しました。");
		return false;
	}

	return true;
}

#if defined(_DEBUG)
bool FWK::Render::Hardware::EnableDebugLayer() const
{
	TypeDEF::ComPtr<ID3D12Debug5> l_debug = nullptr;
	
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