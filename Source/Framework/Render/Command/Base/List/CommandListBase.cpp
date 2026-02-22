#include "CommandListBase.h"

FWK::Render::CommandListBase::CommandListBase(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType) :
	k_hardware             (a_hardware),
	k_createCommandListType(a_createCommandListType)
{}
FWK::Render::CommandListBase::~CommandListBase() = default;

void FWK::Render::CommandListBase::Init()
{
	m_commandList.Reset();
}
bool FWK::Render::CommandListBase::Create()
{
	const auto& l_device = k_hardware.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	// 閉じた状態のコマンドリストを作成
	auto l_hr = l_device->CreateCommandList1(k_hardware.GetGPUNodeMask(),
											 k_createCommandListType,
											 D3D12_COMMAND_LIST_FLAG_NONE, 
											 IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}