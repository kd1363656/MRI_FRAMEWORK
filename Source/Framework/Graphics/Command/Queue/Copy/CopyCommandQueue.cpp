#include "CopyCommandQueue.h"

FWK::Graphics::CopyCommandQueue::CopyCommandQueue() : 
	CommandQueueBase(k_createCommandListType)
{}
FWK::Graphics::CopyCommandQueue::~CopyCommandQueue() = default;