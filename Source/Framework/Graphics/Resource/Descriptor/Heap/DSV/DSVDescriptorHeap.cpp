#include "DSVDescriptorHeap.h"

// DepthStencilViewヒープはDirectX12の仕様上ShaderVisibleに対応していない
// DepthStencildViewはOMSetRenderTargetsでCPUディスクリプタハンドルを使って設定する
FWK::Graphics::DSVDescriptorHeap::DSVDescriptorHeap() : 
	DescriptorHeapBase(k_createDescriptorHeapType, Constant::k_useCPUOnlyDescriptorHeap, Constant::k_notUseShaderVisibleDescriptorHeap)
{}
FWK::Graphics::DSVDescriptorHeap::~DSVDescriptorHeap() = default;