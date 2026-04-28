#include "DirectCommandAllocator.h"

FWK::Graphics::DirectCommandAllocator::DirectCommandAllocator() : 
	CommandAllocatorBase(Constant::k_directCommandListType)
{}
FWK::Graphics::DirectCommandAllocator::~DirectCommandAllocator() = default;