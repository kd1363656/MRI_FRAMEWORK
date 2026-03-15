#include "Device.h"

void FWK::Graphics::Device::Init()
{
	m_device.Reset();
}
bool FWK::Graphics::Device::Create(const Factory& a_factory)
{
	// ファクトリーが未作製ならGPU列挙ができないのでreturn
	const auto& l_factory = a_factory.GetFactory();

	if (!l_factory)
	{
		assert(false && "ファクトリの作成ができておらず、デバイスの作成に失敗しました。");
		return false;
	}

	// 現在調査中のGPUアダプターを一時的に受け取る変数
	Microsoft::WRL::ComPtr<IDXGIAdapter4> l_adapter = nullptr;

	// 最終的に使うGPUアダプターの格納先
	Microsoft::WRL::ComPtr<IDXGIAdapter4> l_selectedAdapter = nullptr;

	// 最終的に採用したフィーチャーレベルを保持する変数
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

	// D3D12CreateDevice(どのGPUを使ってデバイスを作るか、
	//					 どのフィーチャーレベルで作りたいか、
	//					 受け取りたいCOMインターフェース型のID、
	//					 作成結果のポインタを書き込むアドレス);

	// 指定した優先条件で、GPU一覧の中からa_index番目のGPUを取り出す関数
	// EnumAdapterByGpuPreference(何番目のGPUアダプターを取得するか、
	//							  どういう種類のGPUを優先して列挙するか、
	//							  受け取りたいCOMインターフェース型のID、
	//							  作成結果のポインタを書き込むアドレス)

	UINT l_adapterIndex = 0U;

	while (SUCCEEDED(l_factory->EnumAdapterByGpuPreference(l_adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(l_adapter.ReleaseAndGetAddressOf()))))
	{
		// 現在取得したGPUの表際情報を受け取る構造体
		auto l_desc = DXGI_ADAPTER_DESC3();

		// 念のため、アダプター取得結果がnullptrならこの要素は無視する
		if (!l_adapter) { continue; }

		// 現在のGPUアダプターの情報を取得する関数
		// GetDesc3(取得したGPU情報を書き込む構造体のアドレス);

		auto l_getDescResult = l_adapter->GetDesc3(&l_desc);

		if (FAILED(l_getDescResult)) { continue; }

		// ソフトウェアアダプターは除外する
		if (l_desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) { continue; }

		// このGPuで度のフィーチャーレベルまで使えるかを高い順に調べる
		for (auto l_level : l_featureLevels)
		{
			// 指定したGPUとフィーチャーレベルでDirectX12デバイスが作成できるかを確認する
			// 第四引数にnullptrを渡しているため実際にデバイスを受け取るのではなく
			// 「この条件でデバイス作成が可能かどうかのテスト」だけを行っている
			auto l_createDeviceResult = D3D12CreateDevice(l_adapter.Get(), l_level, __uuidof(ID3D12Device), nullptr);
			
			// もし作成テストに失敗したらもう一段階フィーチャーレベルを落として再確認する
			if (FAILED(l_createDeviceResult)) { continue; }

			// 指定のGPUは指定のフィーチャーレベルで使用可能なので採用候補として保存する
			l_selectedAdapter      = l_adapter;
			l_selectedFeatureLevel = l_level;
			l_isFound              = true;
			break;
		}

		// 所要可能なGPUが見つかったので、これ以上捜さずループ終了
		if (l_isFound) { break; }

		++l_adapterIndex;
	}

	// 使用可能なGPUが一つも見つからなかった場合は失敗
	if (!l_isFound)
	{
		assert(false && "対応するGPUが見つかっておらず、デバイスの作成に失敗しました。");
		return false;
	}

	// 事前チェックで使用可能と分かったGPUとフィーチャーレベルを使って、
	// 実際にDirectX12デバイスを作成する
	auto l_hr = D3D12CreateDevice(l_selectedAdapter.Get(), l_selectedFeatureLevel, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()));

	// 作成が失敗した場合はエラー
	if (FAILED(l_hr))
	{
		assert(false && "デバイスの作成に失敗しました。");
		return false;
	}

	return true;
}