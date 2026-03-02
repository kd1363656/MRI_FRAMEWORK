#include "Factory.h"

void FWK::Graphics::Factory::Init()
{
	m_factory.Reset();
}
bool FWK::Graphics::Factory::Create()
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