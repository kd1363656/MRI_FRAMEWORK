#include "SRVDescriptorHeap.h"

FWK::Graphics::SRVDescriptorHeap::SRVDescriptorHeap(const Device& a_device) : 
	DescriptorHeapBase(a_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true)
{}
FWK::Graphics::SRVDescriptorHeap::~SRVDescriptorHeap() = default;