#include "CopyCommandQueue.h"

FWK::Graphics::CopyCommandQueue::CopyCommandQueue() : 
	CommandQueueBase(Constant::k_copyCommandListType)
{}
FWK::Graphics::CopyCommandQueue::~CopyCommandQueue() = default;