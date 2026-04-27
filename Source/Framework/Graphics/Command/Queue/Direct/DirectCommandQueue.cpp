#include "DirectCommandQueue.h"

FWK::Graphics::DirectCommandQueue::DirectCommandQueue() : 
	CommandQueueBase(k_createCommandListType)
{}
FWK::Graphics::DirectCommandQueue::~DirectCommandQueue() = default;