#include "CopyCommandAllocator.h"

FWK::Graphics::CopyCommandAllocator::CopyCommandAllocator() : 
	CommandAllocatorBase(D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandAllocator::~CopyCommandAllocator() = default;