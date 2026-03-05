#include "ResourceContext.h"

FWK::Graphics::ResourceContext::ResourceContext(const Device& a_device) :
	m_descriptorHeapContext (a_device),
	m_copyCommandQueue      (a_device),
	m_copyCommandAllocator  (a_device),
	m_copyCommandList       (a_device),
	m_srvDescriptorAllocator(m_descriptorHeapContext.GetSRVDescriptorHeap())
{}
FWK::Graphics::ResourceContext::~ResourceContext() = default;

void FWK::Graphics::ResourceContext::Init()
{
	m_descriptorHeapContext.Init();

	m_copyCommandQueue.Init    ();
	m_copyCommandAllocator.Init();
	m_copyCommandList.Init     ();

	m_srvDescriptorAllocator.Init();
}
bool FWK::Graphics::ResourceContext::Create(const SwapChain& a_swapChain)
{
	if (!m_descriptorHeapContext.Create(a_swapChain))
	{
		assert(false && "ディスクリプタヒープコンテキストの作成に失敗しました。");
		return false;
	}

	if (!m_copyCommandQueue.Create())
	{
		assert(false && "コピーコマンドキューの作成に失敗しました。");
		return false;
	}

	if (!m_copyCommandAllocator.Create())
	{
		assert(false && "コピーコマンドアロケータの作成に失敗しました。");
		return false;
	}

	if (!m_copyCommandList.Create())
	{
		assert(false && "コピーコマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}