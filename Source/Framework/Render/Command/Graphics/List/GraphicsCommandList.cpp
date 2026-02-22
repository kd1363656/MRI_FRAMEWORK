#include "GraphicsCommandList.h"

FWK::Render::GraphicsCommandList::GraphicsCommandList(const GraphicsCommandContext& a_graphicsCommandContext) : 
	CommandListBase         (a_graphicsCommandContext.GetHardware(), CommonConstant::k_createCommandListTypeDirect),
	k_graphicsCommandContext(a_graphicsCommandContext)
{}
FWK::Render::GraphicsCommandList::~GraphicsCommandList() = default;