#include "CommandQueueBase.h"

FWK::Graphics::CommandQueueBase::CommandQueueBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_createCommandListType(a_createCommandListType),

	m_commandQueue(nullptr)
{}
FWK::Graphics::CommandQueueBase::~CommandQueueBase() = default;