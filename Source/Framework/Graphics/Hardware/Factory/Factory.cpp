#include "Factory.h"

void FWK::Graphics::Factory::Init()
{
	m_factory.Reset();
}
bool FWK::Graphics::Factory::Create()
{
	// DXGIファクトリー作成時に渡すオプションフラグ
	// 通常は0でよいが、デバッグ時はデバッグ用フラグを追加する
	UINT l_flags = 0U;

#if defined(_DEBUG)
	// Debugビルド時はDXGI_CREATE_FACTORY_DEBUGを有効にする
	// これにより、DXGI関連の問題をデバックレイヤーが検出しやすくなる
	l_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	// DXGIファクトリーを作成する関数
	// CreateDXGIFactory2(ファクトリー作成時のオプションを指定する、
	//					  受け取りたいCOMインターフェース型のID、
	//					  作成結果のポインタを書き込むアドレス);

	auto hr = CreateDXGIFactory2(l_flags, IID_PPV_ARGS(m_factory.ReleaseAndGetAddressOf()));

	if (FAILED(hr))
	{
		assert(false && "ファクトリーの作成に失敗しました");
		return false;
	}
	
	return true;
}