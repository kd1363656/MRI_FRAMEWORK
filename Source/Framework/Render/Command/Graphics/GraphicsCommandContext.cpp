#include "GraphicsCommandContext.h"

FWK::Render::GraphicsCommandContext::GraphicsCommandContext(const Hardware& a_hardware) :
	k_hardware            (a_hardware),
	m_graphicsCommandQueue(*this, k_createCommandListType)
{}
FWK::Render::GraphicsCommandContext::~GraphicsCommandContext() = default;