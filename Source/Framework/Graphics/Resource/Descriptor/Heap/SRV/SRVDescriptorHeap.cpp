#include "SRVDescriptorHeap.h"

FWK::Graphics::SRVDescriptorHeap::SRVDescriptorHeap() :
	DescriptorHeapBase(Constant::k_cbvSRVUAVDescriptorHeapType, Constant::k_useCPUOnlyDescriptorHeap, Constant::k_useShaderVisibleDescriptorHeap)
{}
FWK::Graphics::SRVDescriptorHeap::~SRVDescriptorHeap() = default;