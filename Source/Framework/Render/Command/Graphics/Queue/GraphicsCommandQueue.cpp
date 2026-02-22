#include "GraphicsCommandQueue.h"

FWK::Render::GraphicsCommandQueue::GraphicsCommandQueue(const GraphicsCommandContext& a_graphicsCommandContext) : 
	CommandQueueBase        (a_graphicsCommandContext.GetHardware(), CommonConstant::k_createCommandListTypeDirect),
	k_graphicsCommandContext(a_graphicsCommandContext)
{}
FWK::Render::GraphicsCommandQueue::~GraphicsCommandQueue() = default;