#include "CommandAllocatorBase.h"

FWK::Graphics::CommandAllocatorBase::CommandAllocatorBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_createCommandListType(a_createCommandListType),

	m_commandAllocator(nullptr),

	m_submittedFenceValue(Constant::k_unusedFenceValue)
{}
FWK::Graphics::CommandAllocatorBase::~CommandAllocatorBase() = default;

bool FWK::Graphics::CommandAllocatorBase::Create(const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	// デバイスが存在しないなら作成できないのでreturn
	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、コマンドアロケータの作成が出来ませんでした。");
		return false;
	}

	// コマンドアロケータを作成する関数
	// CreateCommandAllocator(このコマンドアロケータが記録対象とするコマンドリストの種類(※注意 : 作成するコマンドリストの種類と合わせる必要がある)、
	//						  受け取りたいCOMインターフェース型のID、
	//					      作成結果のポインタを書き込むアドレス);

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

	// コマンドアロケータを再利用できる状態に戻す関数
	// Reset();

	m_commandAllocator->Reset();
}