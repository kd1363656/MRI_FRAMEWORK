#include "SRVDescriptorHeap.h"

FWK::Graphics::SRVDescriptorHeap::SRVDescriptorHeap() :
	DescriptorHeapBase(k_descriptorHeapType, k_isUseCPUOnly, k_isUseShaderVisible)
{}
FWK::Graphics::SRVDescriptorHeap::~SRVDescriptorHeap() = default;