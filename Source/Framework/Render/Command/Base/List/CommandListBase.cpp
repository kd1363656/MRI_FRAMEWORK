#include "CommandListBase.h"

void FWK::Render::CommandListBase::Init()
{
	m_commandList.Reset();
}
bool FWK::Render::CommandListBase::Create(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType)
{
	const auto& l_device = a_hardware.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	// 閉じた状態のコマンドリストを作成
	auto l_hr = l_device->CreateCommandList1(a_hardware.GetGPUNodeMask(),
											 a_createCommandListType,
											 D3D12_COMMAND_LIST_FLAG_NONE, 
											 IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}