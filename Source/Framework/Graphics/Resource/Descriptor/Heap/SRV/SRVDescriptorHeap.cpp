#include "SRVDescriptorHeap.h"

FWK::Graphics::SRVDescriptorHeap::SRVDescriptorHeap() :
	DescriptorHeapBase(k_createDescriptorHeapType, Constant::k_useCPUOnlyDescriptorHeap, Constant::k_useShaderVisibleDescriptorHeap)
{}
FWK::Graphics::SRVDescriptorHeap::~SRVDescriptorHeap() = default;