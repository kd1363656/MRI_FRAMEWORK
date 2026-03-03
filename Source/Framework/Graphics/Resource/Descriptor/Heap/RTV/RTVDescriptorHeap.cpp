#include "RTVDescriptorHeap.h"

FWK::Graphics::RTVDescriptorHeap::RTVDescriptorHeap(const Device& a_device) : 
	DescriptorHeapBase(a_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false)
{}
FWK::Graphics::RTVDescriptorHeap::~RTVDescriptorHeap() = default;