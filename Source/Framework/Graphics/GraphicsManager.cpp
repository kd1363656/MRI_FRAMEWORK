#include "GraphicsManager.h"

void FWK::Graphics::GraphicsManager::Init() const
{
#if defined(_DEBUG)
	if (!EnableDebugLayer())
	{
		assert(false && "デバッグレイヤーの有効化に失敗しました。");
		return;
	}
#endif
}
bool FWK::Graphics::GraphicsManager::Create() const
{
    return true;
}

bool FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	TypeAlias::ComPtr<ID3D12Debug5> l_debug = nullptr;

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