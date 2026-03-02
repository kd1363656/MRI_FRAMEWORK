#include "CommandListBase.h"

FWK::Graphics::CommandListBase::CommandListBase(const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_device               (a_device),
	k_createCommandListType(a_createCommandListType)
{}
FWK::Graphics::CommandListBase::~CommandListBase() = default;

void FWK::Graphics::CommandListBase::Init()
{
	m_commandList.Reset();
}
bool FWK::Graphics::CommandListBase::Create()
{
	const auto& l_device = k_device.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、コマンドリストの作成に失敗しました。");
		return false;
	}

	// 閉じた状態のコマンドリストを作成
	auto l_hr = l_device->CreateCommandList1(k_device.GetGPUNodeMask(),
											 k_createCommandListType,
											 D3D12_COMMAND_LIST_FLAG_NONE, 
											 IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "コマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}