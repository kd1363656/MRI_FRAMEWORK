#include "GraphicsCommandAllocator.h"

FWK::Render::GraphicsCommandAllocator::GraphicsCommandAllocator(const GraphicsCommandContext& a_graphicsCommandContext) :
	CommandAllocatorBase    (a_graphicsCommandContext.GetHardware(), CommonConstant::k_createCommandListTypeDirect),
	k_graphicsCommandContext(a_graphicsCommandContext)
{}
FWK::Render::GraphicsCommandAllocator::~GraphicsCommandAllocator() = default;