#include "DirectCommandQueue.h"

FWK::Graphics::DirectCommandQueue::DirectCommandQueue() : 
	CommandQueueBase(D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandQueue::~DirectCommandQueue() = default;