#include "RTVDescriptorHeap.h"

// RenderTargetViewヒープはDirectX12の仕様上ShaderVisibleに対応していない
// RenderTargetViewはOMSetRenderTargetsでCPUディスクリプタハンドルを使って設定する
FWK::Graphics::RTVDescriptorHeap::RTVDescriptorHeap() : 
	DescriptorHeapBase(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, false)
{}
FWK::Graphics::RTVDescriptorHeap::~RTVDescriptorHeap() = default;