#include "ResourceContext.h"

FWK::Graphics::ResourceContext::ResourceContext(const Device& a_device) :
	m_descriptorHeapContext(a_device)
{}
FWK::Graphics::ResourceContext::~ResourceContext() = default;

void FWK::Graphics::ResourceContext::Init()
{
	m_descriptorHeapContext.Init();
}
bool FWK::Graphics::ResourceContext::Create(const SwapChain& a_swapChain)
{
	if (!m_descriptorHeapContext.Create(a_swapChain))
	{
		assert(false && "ディスクリプタヒープコンテキストの作成に失敗しました。");
		return false;
	}

	return true;
}