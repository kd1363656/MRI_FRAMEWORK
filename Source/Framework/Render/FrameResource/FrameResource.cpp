#include "FrameResource.h"

FWK::Render::FrameResource::FrameResource(const GraphicsCommandContext& a_graphicsCommandContext) : 
	m_graphicsCommandAllocator(a_graphicsCommandContext),
	m_fenceValue			  (CommonConstant::k_initialFenceValue)
{}
FWK::Render::FrameResource::~FrameResource() = default;

void FWK::Render::FrameResource::Init()
{
	m_graphicsCommandAllocator.Init();

	m_fenceValue = CommonConstant::k_initialFenceValue;
}
bool FWK::Render::FrameResource::Create()
{
	if (!m_graphicsCommandAllocator.Create())
	{
		assert(false && "コマンドアロケータの作成に失敗しました。");
		return false;
	}

	return true;
}