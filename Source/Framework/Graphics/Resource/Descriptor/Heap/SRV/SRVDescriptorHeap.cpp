#include "SRVDescriptorHeap.h"

FWK::Graphics::SRVDescriptorHeap::SRVDescriptorHeap() :
	DescriptorHeapBase(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, true)
{}
FWK::Graphics::SRVDescriptorHeap::~SRVDescriptorHeap() = default;