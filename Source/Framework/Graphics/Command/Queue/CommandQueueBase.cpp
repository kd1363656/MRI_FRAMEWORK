#include "CommandQueueBase.h"

FWK::Graphics::CommandQueueBase::CommandQueueBase(const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_device               (a_device),
	k_createCommandListType(a_createCommandListType),
	m_commandQueue         (nullptr),
	m_fence                (a_device)
{}
FWK::Graphics::CommandQueueBase::~CommandQueueBase()
{
	// GPUと完全同期をとってDirectX12のDeviceが
	// Releaseされてもいい状態にする
	m_fence.WaitForGPUIdle(m_commandQueue.Get());
}

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

void FWK::Graphics::CommandQueueBase::ExecuteCommandLists(const CommandListBase& a_commandList) const
{
	const auto& l_commandQueue = GetCommandQueue		     ();
	const auto& l_commandList  = a_commandList.GetCommandList();
	
	if (!l_commandQueue)
	{
		assert(false && "コマンドキューが作成されておらず、コマンド実行処理が行えませんでした");
		return;
	}

	if (!l_commandList)
	{
		assert(false && "コマンドリストが作成されておらず、コマンド実行処理が行えませんでした");
		return;
	}

	// このキューと違うコマンドリストタイプならreturn
	if (GetCreateCommandListType() != a_commandList.GetCreateCommandListType())
	{
		assert(false && "コマンドリストとコマンドキューのコマンドリストタイプが違います、コマンド実行処理が行えませんでした");
		return;
	}

	// Closeで命令受付をやめてコマンドリストを実行
	l_commandList->Close();

	ID3D12CommandList* l_list[] = { l_commandList.Get() };

	l_commandQueue->ExecuteCommandLists(_countof(l_list), l_list);
}

void FWK::Graphics::CommandQueueBase::EnsureAllocatorAvailable(const CommandAllocatorBase& a_commandAllocator)
{
	m_fence.WaitForFenceValue(a_commandAllocator.GetSubmittedFenceValue());
}
void FWK::Graphics::CommandQueueBase::SignalAndTrackAllocator(CommandAllocatorBase& a_commandAllocator)
{
	const auto& l_fence = m_fence.GetFence();

	if (!l_fence)
	{
		assert(false && "フェンスが作成されておらずGPUとの同期処理が行えませんでした。");
		return;
	}

	if (!m_commandQueue)
	{
		assert(false && "ダイレクトコマンドキューが作成されておらずGPUとの同期処理が行えませんでした。");
		return;
	}

	// "FenceValue"を進めて、このフレームの完了目標として保存
	m_fence.SetFenceValue(m_fence.GetFenceValue() + k_incrementFenceValue);

	// ※重要
	// 更新したフェンス値を持たせて置く、こうすることで次のフレームでフェンス値を超えていない場合
	// GPUとの同期をとらなくていいためCPUとGPUの並列処理性を発揮することができる
	a_commandAllocator.SetSubmittedFenceValue(m_fence.GetFenceValue());

	// コマンドキュー内でこの位置までの命令が実行完了したら
	// フェンス値をGetFenceValueに更新する命令をGPUに追加
	m_commandQueue->Signal(l_fence.Get(), m_fence.GetFenceValue());
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