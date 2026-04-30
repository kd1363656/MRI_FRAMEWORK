#include "SRVDescriptorHeap.h"

FWK::Graphics::SRVDescriptorHeap::SRVDescriptorHeap() :
	DescriptorHeapBase(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Constant::k_useCPUOnlyDescriptorHeap, Constant::k_useShaderVisibleDescriptorHeap)
{}
FWK::Graphics::SRVDescriptorHeap::~SRVDescriptorHeap() = default;