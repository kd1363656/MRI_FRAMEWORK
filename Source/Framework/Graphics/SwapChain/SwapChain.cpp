#include "SwapChain.h"

void FWK::Graphics::SwapChain::Deserialize(const nlohmann::json& a_rootJson)
{
}

bool FWK::Graphics::SwapChain::Create(const HWND& a_hwnd, const Device& a_device, const DirectCommandQueue& a_directCommandQueue, const Struct::WindowCONFIG& a_windowCONFIG, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool)
{
	return false;
}

void FWK::Graphics::SwapChain::PostCreateSetup(const HWND& a_hWND, const Factory& a_factory)
{
}

nlohmann::json FWK::Graphics::SwapChain::Serialize() const
{
	return nlohmann::json();
}

bool FWK::Graphics::SwapChain::CreateSwapChain(const HWND& a_hwnd, const Factory& a_factory, const DirectCommandQueue& a_directCommandQueue, const Struct::WindowCONFIG& a_windowCONFIG)
{
	return false;
}

bool FWK::Graphics::SwapChain::CreateBackBufferList(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool)
{
	return false;
}
