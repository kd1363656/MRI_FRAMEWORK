#include "Device.h"

FWK::Graphics::Device::Device(const Factory& a_factory) : 
	k_factory            (a_factory),
	m_device             (nullptr),
	m_deviceJsonConverter(*this),
	m_gpuNodeMask        (0U)
{}
FWK::Graphics::Device::~Device() = default;

void FWK::Graphics::Device::Init()
{
	m_device.Reset();

	m_gpuNodeMask = CommonConstant::k_defaultGPUNodeMask;
}
bool FWK::Graphics::Device::Create()
{
	const auto& l_factory = k_factory.GetFactory();

	// ファクトリーが作成できていなければグラフィックスカードの列挙ができないためreturn
	if (!l_factory)
	{
		assert(false && "ファクトリーが作成がされておらず、デバイスの作成に失敗しました。");
		return false;
	}

	ComPtr<IDXGIAdapter4> l_adapter         = nullptr;
	ComPtr<IDXGIAdapter4> l_selectedAdapter = nullptr;

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
	for (UINT l_i = 0U; l_factory->EnumAdapterByGpuPreference(l_i,
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
		assert(false && "対応するGPUが見つかっておらず、デバイスの作成に失敗しました。");
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

void FWK::Graphics::Device::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_deviceJsonConverter.Deserialize(a_rootJson);
}
nlohmann::json FWK::Graphics::Device::Serialize()
{
	return m_deviceJsonConverter.Serialize();
}