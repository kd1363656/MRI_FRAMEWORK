#include "RTVDescriptorHeap.h"

// RenderTargetViewヒープはDirectX12の仕様上ShaderVisibleに対応していない
// RenderTargetViewはOMSetRenderTargetsでCPUディスクリプタハンドルを使って設定する
FWK::Graphics::RTVDescriptorHeap::RTVDescriptorHeap() : 
	DescriptorHeapBase(k_createDescriptorHeapType, Constant::k_useCPUOnlyDescriptorHeap, Constant::k_notUseShaderVisibleDescriptorHeap)
{}
FWK::Graphics::RTVDescriptorHeap::~RTVDescriptorHeap() = default;