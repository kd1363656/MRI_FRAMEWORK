#include "SwapChain.h"

void FWK::Graphics::SwapChain::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_swapChainJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::SwapChain::Create(const HWND&							   a_hwnd, 
									  const Factory&						   a_factory,
									  const DirectCommandQueue&				   a_directCommandQueue,
									  const Struct::WindowCONFIG&			   a_windowCONFIG, 
											DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool)
{
	if (!CreateSwapChain(a_hwnd,
						 a_factory,
						 a_directCommandQueue,
						 a_windowCONFIG))
	{
		assert(false && "スワップチェインの作成に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::SwapChain::Serialize() const
{
	return m_swapChainJsonConverter.Serialize(*this);
}

void FWK::Graphics::SwapChain::ResizeBackBufferList(const std::size_t a_backBufferNum)
{
	m_backBufferList.resize(a_backBufferNum);
}

bool FWK::Graphics::SwapChain::CreateSwapChain(const HWND&                 a_hwnd, 
											   const Factory&              a_factory,
											   const DirectCommandQueue&   a_directCommandQueue, 
											   const Struct::WindowCONFIG& a_windowConfig)
{
	const auto& l_factory = a_factory.GetREFFactory();

	if (!l_factory)
	{
		assert(false && "ファクトリーの作成がされておらず、スワップチェインの作成が出来ません");
		return false;
	}

	// スワップチェインは描画結果を表示するための仕組みなので、描画に使う
	// Directタイプのコマンドキューである必要がある
	// CopyやCompute用のコマンドキューでは画面表示用のスワップチェインを作ることはできないのでreturn
	if (a_directCommandQueue.GetVALCreateCommandListType() != D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		assert(false && "コマンドキューのコマンドリストタイプは描画可能なDirectでないといけません");
		return false;
	}

	const auto& l_commandQueue = a_directCommandQueue.GetREFCommandQueue();

	if (!l_commandQueue)
	{
		assert(false && "コマンドキューの作成がされていないためスワップチェインが作成できません");
		return false;
	}

	// バックバッファ枚数が0だと表示用バッファを一枚も持てないので作成できない。
	// 通常は2枚以上にすることが多い
	if (m_backBufferList.empty())
	{
		assert(false && "バックバッファリストの中身が空になっておりスワップチェインを作成できませんでした。");
		return false;
	}

	// スワップチェインの作成設定をまとめた構造体
	// バッファサイズ、バッファ枚数、入れ替え方式などをここで指定する。
	auto l_desc = DXGI_SWAP_CHAIN_DESC1();

	// バックバッファ1枚当たりの横幅
	l_desc.Width = a_windowConfig.m_width;

	// バックバッファ一枚当たりの縦幅
	l_desc.Height = a_windowConfig.m_height;

	// バックバッファの画素フォーマット
	// 1ピクセルをどういう並びの色データとして持つかを表す。
	// R8G8B8A8_UNORMは、RGBA各8bitの標準的なカラー形式
	// ※注意 : RTVを作るときのフォーマットとも基本的にそろえる必要がある
	l_desc.Format = k_swapChainFormat;

	// 立体視表示を使うかどうか
	// 通常の2D/3Dでは使わないためFALSEにする
	l_desc.Stereo = FALSE;

	// マルチサンプリング数
	// 1はMSAAを使わない通常設定
	// 4などにするとアンチエイリアス用途になるが、
	// スワップチェインのバックバッファでは1を使うことが多い
	l_desc.SampleDesc.Count = k_defaultSampleCount;

	// マルチサンプリング品質レベル
	// Count = 1の通常設定では0を使うことが多い
	l_desc.SampleDesc.Quality = k_defaultSampleQuality;

	// このバッファを何に使うかを指定する
	// DXGI_USAGE_RENDER_TARGET_OUTPUTは
	// 「レンダーターゲットとして描画結果を書き込む用途」で使う設定
	l_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// バックバッファの枚数
	// 2ならダブルバッファ、3ならトリプルバッファになる。
	// 枚数を増やすとGPU / CPUとの並列性が上がる場合があるが、
	// その分メモリも使用する
	l_desc.BufferCount = static_cast<UINT>(m_backBufferList.size());

	// バックバッファサイズと表示サイズが違うときの拡大縮小方法
	//STRETCHは表示先に合わせて引き延ばす設定
	l_desc.Scaling = DXGI_SCALING_STRETCH;

	// バックバッファの入れ替え方式
	// FLIP_DISCARDはDirectX12で一般的なフリップモデルの設定
	// 昔のDISCARD \ SEQUENTIALより現代的な方法で、基本的にこれを使うことが多い
	l_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// α合成の扱い。
	// 通常のウィンドウ描画ではUNSPECIFIEDで問題ないことが多い
	l_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// 追加オプションフラグ
	// 今回は特別な機能を使わないので0
	// 例えばtearing対応などを入れる場合はここにフラグを追加する
	l_desc.Flags = 0U;

	TypeAlias::ComPtr<IDXGISwapChain1> l_swapChain = nullptr;

	// スワップチェイン作成関数
	// CreateSwapChainForHwnd(表示に使うコマンドキュー、
	//						  表示先ウィンドウハンドル、
	//						  スワップチェイン設定、
	//						  フルスクリーン詳細設定(nullptrなら未設定)、
	//						  出力先モニター制限(nullptrなら未指定)
	//						  作成したスワップチェインの受け取り先);

	auto l_hr = l_factory->CreateSwapChainForHwnd(l_commandQueue.Get(), 
												  a_hwnd, 
												  &l_desc, 
												  nullptr,
												  nullptr,
												  l_swapChain.ReleaseAndGetAddressOf());

	if (FAILED(l_hr))
	{
		assert(false && "スワップチェインの作成に失敗しました。");
		return false;
	}

	// As関数はCOMのQueryInterfaceを使って安全に型変換する。
	// ここではIDXGISwapChain1からメンバーが持つ方へ変換している
	l_hr = l_swapChain.As(&m_swapChain);

	if (FAILED(l_hr) || !m_swapChain)
	{
		assert(false && "スワップチェインの型変換に失敗しました、スワップチェインの作成が行えません。");
		return false;
	}

	return true;
}