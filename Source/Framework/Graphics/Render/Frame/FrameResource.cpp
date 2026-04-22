#include "FrameResource.h"

bool FWK::Graphics::FrameResource::Create(const Device& a_device)
{
	if (!m_directCommandAllocator.Create(a_device))
	{
		assert(false && "ダイレクトコマンドアロケータの作成処理に失敗しました。");
		return false;
	}

	return true;
}