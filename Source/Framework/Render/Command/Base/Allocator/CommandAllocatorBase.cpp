#include "CommandAllocatorBase.h"

FWK::Render::CommandAllocatorBase::CommandAllocatorBase(const Hardware& a_hardware) : 
	k_hardware(a_hardware)
{}
FWK::Render::CommandAllocatorBase::~CommandAllocatorBase() = default;

void FWK::Render::CommandAllocatorBase::Init()
{
	m_commandAllocator.Reset();
}
bool FWK::Render::CommandAllocatorBase::Create(const D3D12_COMMAND_LIST_TYPE a_createCommandListType)
{
	const auto& l_device = k_hardware.GetDevice();

	if (!l_device) 
	{
		assert(false && "デバイスの作成に失敗しており、コマンドアロケーターを作成できません。");
		return false; 
	}

	auto l_hr = l_device->CreateCommandAllocator(a_createCommandListType, IID_PPV_ARGS(m_commandAllocator.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "コマンドアロケータの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Render::CommandAllocatorBase::Reset() const
{
	if (!m_commandAllocator)
	{
		assert(false && "コマンドアロケータの作成に失敗しており、リセット処理を行うことができませんでした。");
		return;
	}

	m_commandAllocator->Reset();
}