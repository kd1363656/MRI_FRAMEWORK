#include "CommandAllocatorBase.h"

FWK::Graphics::CommandAllocatorBase::CommandAllocatorBase(const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_device               (a_device),
	k_createCommandListType(a_createCommandListType)
{}
FWK::Graphics::CommandAllocatorBase::~CommandAllocatorBase() = default;

void FWK::Graphics::CommandAllocatorBase::Init()
{
	m_commandAllocator.Reset();
}
bool FWK::Graphics::CommandAllocatorBase::Create()
{
	const auto& l_device = k_device.GetDevice();

	if (!l_device) 
	{
		assert(false && "デバイスの作成に失敗しており、コマンドアロケータを作成できません。");
		return false; 
	}

	auto l_hr = l_device->CreateCommandAllocator(k_createCommandListType, IID_PPV_ARGS(m_commandAllocator.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "コマンドアロケータの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::CommandAllocatorBase::Reset() const
{
	if (!m_commandAllocator)
	{
		assert(false && "コマンドアロケータの作成に失敗しており、リセット処理を行うことができませんでした。");
		return;
	}

	m_commandAllocator->Reset();
}