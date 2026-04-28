#include "CopyCommandAllocator.h"

FWK::Graphics::CopyCommandAllocator::CopyCommandAllocator() : 
	CommandAllocatorBase(Constant::k_copyCommandListType)
{}
FWK::Graphics::CopyCommandAllocator::~CopyCommandAllocator() = default;