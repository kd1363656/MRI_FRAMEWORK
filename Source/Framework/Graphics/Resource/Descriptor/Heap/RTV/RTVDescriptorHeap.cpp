#include "RTVDescriptorHeap.h"

// RenderTargetViewヒープはDirectX12の仕様上ShaderVisibleに対応していない
// RenderTargetViewはOMSetRenderTargetsでCPUディスクリプタハンドルを使って設定する
FWK::Graphics::RTVDescriptorHeap::RTVDescriptorHeap() : 
	DescriptorHeapBase(k_descriptorHeapType, k_isUseCPUOnly, k_isUseShaderVisible)
{}
FWK::Graphics::RTVDescriptorHeap::~RTVDescriptorHeap() = default;