#include "FrameResource.h"

FWK::Graphics::FrameResource::FrameResource(const Device& a_device) : 
	m_directCommandAllocator(a_device)
{}
FWK::Graphics::FrameResource::~FrameResource() = default;

void FWK::Graphics::FrameResource::Init()
{
	m_directCommandAllocator.Init();
}
bool FWK::Graphics::FrameResource::Create()
{
	if (!m_directCommandAllocator.Create())
	{
		assert(false && "コマンドアロケータの作成に失敗しました。");
		return false;
	}

	return true;
}