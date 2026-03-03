#include "CommandQueueBase.h"

FWK::Graphics::CommandQueueBase::CommandQueueBase(const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_device               (a_device),
	k_createCommandListType(a_createCommandListType),
	m_commandQueue         (nullptr),
	m_fence                (a_device, *this)
{}
FWK::Graphics::CommandQueueBase::~CommandQueueBase() = default;

void FWK::Graphics::CommandQueueBase::Init()
{
	m_commandQueue.Reset();
	m_fence.Init        ();
}
bool FWK::Graphics::CommandQueueBase::Create()
{
	if (!CreateCommandQueue())
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	if (!CreateFence())
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::CommandQueueBase::CreateCommandQueue()
{
	const auto& l_device = k_device.GetDevice().Get();

	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、コマンドキューの作成ができません。");
		return false;
	}

	// コマンドキューのパラメータを設定
	auto l_desc = D3D12_COMMAND_QUEUE_DESC();

	l_desc.Type     = k_createCommandListType;			   // ※重要 : 実行するコマンドアロケータ及びコマンドリストはこのキューのTypeと合わせる必要がある
	l_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 優先度
	l_desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;	   // オプション
	l_desc.NodeMask = k_device.GetGPUNodeMask();		   // 単一GPU想定のため通常は0か1を選択、

	auto l_hr = l_device->CreateCommandQueue(&l_desc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::CommandQueueBase::CreateFence()
{
	if (!m_fence.Create())
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	return true;
}