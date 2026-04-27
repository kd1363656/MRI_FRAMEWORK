#include "DirectCommandAllocator.h"

FWK::Graphics::DirectCommandAllocator::DirectCommandAllocator() : 
	CommandAllocatorBase(k_createCommandListType)
{}
FWK::Graphics::DirectCommandAllocator::~DirectCommandAllocator() = default;