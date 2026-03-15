#include "Hardware.h"

void FWK::Graphics::Hardware::Init()
{
	m_factory.Init();
	m_device.Init();
}
bool FWK::Graphics::Hardware::Create()
{
	if (!m_factory.Create())
	{
		assert(false && "ファクトリーの作成に失敗しました。");
		return false;
	}

	if (!m_device.Create(m_factory))
	{
		assert(false && "ファクトリーの作成に失敗しました。");
		return false;
	}

	return true;
}