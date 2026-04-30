#include "CopyCommandQueue.h"

FWK::Graphics::CopyCommandQueue::CopyCommandQueue() : 
	CommandQueueBase(D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandQueue::~CopyCommandQueue() = default;