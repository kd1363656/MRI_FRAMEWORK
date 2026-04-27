#include "CopyCommandAllocator.h"

FWK::Graphics::CopyCommandAllocator::CopyCommandAllocator() : 
	CommandAllocatorBase(k_createCommandListType)
{}
FWK::Graphics::CopyCommandAllocator::~CopyCommandAllocator() = default;