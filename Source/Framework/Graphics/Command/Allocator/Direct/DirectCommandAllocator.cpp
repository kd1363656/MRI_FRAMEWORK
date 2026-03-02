#include "DirectCommandAllocator.h"

FWK::Graphics::DirectCommandAllocator::DirectCommandAllocator(const Device& a_device) : 
	CommandAllocatorBase(a_device, D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandAllocator::~DirectCommandAllocator() = default;