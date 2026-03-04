#include "CopyCommandQueue.h"

FWK::Graphics::CopyCommandQueue::CopyCommandQueue(const Device& a_device) : 
	CommandQueueBase(a_device, D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandQueue::~CopyCommandQueue() = default;