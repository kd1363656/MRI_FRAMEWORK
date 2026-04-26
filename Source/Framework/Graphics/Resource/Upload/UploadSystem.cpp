#include "UploadSystem.h"

bool FWK::Graphics::UploadSystem::Create(const Device& a_device)
{
	if (!m_copyCommandQueue.Create(a_device))
	{
		assert(false && "コピーコマンドキュー作成処理に失敗しました。");
		return false;
	}

	if (!m_copyCommandAllocator.Create(a_device))
	{
		assert(false && "コピーコマンドアロケータ作成処理に失敗しました。");
		return false;
	}

	if (!m_copyCommandList.Create(a_device))
	{
		assert(false && "コピーコマンドリスト作成処理に失敗しました。");
		return false;
	}

	return true;
}