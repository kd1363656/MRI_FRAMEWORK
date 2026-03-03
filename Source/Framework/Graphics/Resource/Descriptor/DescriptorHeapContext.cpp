#include "DescriptorHeapContext.h"

FWK::Graphics::DescriptorHeapContext::DescriptorHeapContext(const Device& a_device) :
	m_rtvDescriptorHeap(a_device)
{}
FWK::Graphics::DescriptorHeapContext::~DescriptorHeapContext() = default;

void FWK::Graphics::DescriptorHeapContext::Init()
{
	m_rtvDescriptorHeap.Init();
}
bool FWK::Graphics::DescriptorHeapContext::Create(const SwapChain& a_swapChain)
{
	// バックバッファの数分ヒープ領域を確保
	if (!m_rtvDescriptorHeap.Create(a_swapChain.GetBackBufferNum()))
	{
		assert(false && "RTVディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	return true;
}