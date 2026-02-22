#include "GraphicsCommandQueue.h"

FWK::Render::GraphicsCommandQueue::GraphicsCommandQueue(const GraphicsCommandContext& a_graphicsCommandContext, const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	CommandQueueBase        (a_graphicsCommandContext.GetHardware(), a_createCommandListType),
	k_graphicsCommandContext(a_graphicsCommandContext)
{}
FWK::Render::GraphicsCommandQueue::~GraphicsCommandQueue() = default;