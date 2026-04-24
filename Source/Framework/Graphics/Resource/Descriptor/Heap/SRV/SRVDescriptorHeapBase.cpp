#include "SRVDescriptorHeapBase.h"

FWK::Graphics::SRVDescriptorHeapBase::SRVDescriptorHeapBase(const bool a_isShaderVisible) : 
	DescriptorHeapBase(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, a_isShaderVisible)
{}
FWK::Graphics::SRVDescriptorHeapBase::~SRVDescriptorHeapBase() = default;