#include "CopyCommandAllocator.h"

FWK::Graphics::CopyCommandAllocator::CopyCommandAllocator(const Device& a_device) : 
	CommandAllocatorBase(a_device, D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandAllocator::~CopyCommandAllocator() = default;