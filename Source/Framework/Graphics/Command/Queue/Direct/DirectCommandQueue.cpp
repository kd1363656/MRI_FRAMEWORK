#include "DirectCommandQueue.h"

FWK::Graphics::DirectCommandQueue::DirectCommandQueue() : 
	CommandQueueBase(Constant::k_directCommandListType)
{}
FWK::Graphics::DirectCommandQueue::~DirectCommandQueue() = default;