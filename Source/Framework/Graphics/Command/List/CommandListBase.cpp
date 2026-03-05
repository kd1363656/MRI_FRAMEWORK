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

void FWK::Graphics::CommandListBase::Reset(const CommandAllocatorBase& a_commandAllocator) const
{
	if (!m_commandList)
	{
		assert(false && "コマンドリストの作成に失敗しており、コマンドリストのリセットが出来ませんでした。");
		return;
	}

	// 自身のコマンドリストタイプと一致しなければreturn
	if (a_commandAllocator.GetCreateCommandListType() != k_createCommandListType)
	{
		assert(false && "コマンドアロケータのコマンドリストタイプが一致しないため、コマンドリストのリセットが出来ませんでした。");
		return;
	}

	const auto& l_commandAllocator = a_commandAllocator.GetCommandAllocator();

	if (!l_commandAllocator)
	{
		assert(false && "コマンドアロケータの作成に失敗しており、コマンドリストのリセットに失敗しました。");
		return;
	}

	m_commandList->Reset(l_commandAllocator.Get(), nullptr);
}

void FWK::Graphics::CommandListBase::TransitionResource(const ComPtr<ID3D12Resource2>& a_resourece, const D3D12_RESOURCE_STATES a_beforState, const D3D12_RESOURCE_STATES a_afterState) const
{
	const auto& l_commandList = GetCommandList();

	if (!a_resourece)
	{
		assert(false && "リソースが作成されておらず、リソースの繊維ができませんでした。");
		return;
	}

	if (!l_commandList)
	{
		assert(false && "コマンドリストが作成されておらず、リソースの状態遷移が出来ませんでした。");
		return;
	}

	// リソースの状態遷移
	// a_beforState -> a_afterState
	const auto& l_barrier = CD3DX12_RESOURCE_BARRIER::Transition(a_resourece.Get(), a_beforState, a_afterState);

	// リソースバリアを実行
	l_commandList->ResourceBarrier(k_sendBarrierNum, &l_barrier);
}