#include "DirectCommandQueueh.h"

FWK::Graphics::DirectCommandQueue::DirectCommandQueue(const Device& a_device) : 
	CommandQueueBase(a_device, D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandQueue::~DirectCommandQueue() = default;