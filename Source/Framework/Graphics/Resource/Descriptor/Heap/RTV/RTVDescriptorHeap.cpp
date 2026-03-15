#include "RTVDescriptorHeap.h"

FWK::Graphics::RTVDescriptorHeap::RTVDescriptorHeap() : 
	DescriptorHeapBase(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false)
{}
FWK::Graphics::RTVDescriptorHeap::~RTVDescriptorHeap() = default;