#include "DirectCommandAllocator.h"

FWK::Graphics::DirectCommandAllocator::DirectCommandAllocator() : 
	CommandAllocatorBase(D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandAllocator::~DirectCommandAllocator() = default;