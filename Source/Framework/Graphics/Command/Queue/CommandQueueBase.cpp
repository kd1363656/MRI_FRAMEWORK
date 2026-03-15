#include "CommandQueueBase.h"

FWK::Graphics::CommandQueueBase::CommandQueueBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_createCommandListType(a_createCommandListType),

	m_commandQueue(nullptr),

	m_fence()
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

	m_fence.Init();
}

bool FWK::Graphics::CommandQueueBase::Create(const Device& a_device)
{
	if(!CreateCommandQueue(a_device))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	if (!CreateFence(a_device))
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::CommandQueueBase::EnsureAllocatorAvailable(const CommandAllocatorBase& a_commandAllocator)
{
	m_fence.WaitForFenceValue(a_commandAllocator.GetSubmittedFenceValue());
}

void FWK::Graphics::CommandQueueBase::ExecuteCommandLists(const CommandListBase& a_commandList) const
{
	const auto& l_commandQueue = GetCommandQueue();
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

	// ExecuteCommandLists()はID3D12CommandList*の配列を受け取るため、
	// 1個だけ実行する場合でも配列にして渡す必要がある
	ID3D12CommandList* l_list[] = { l_commandList.Get() };

	// ExecuteCommandLists(実行するコマンドリストの数、
	//					   コマンドリスト配列の先頭アドレス)

	// ※注意 : ExecuteCommandListをする前にコマンドリストはClose()関数を呼び出す必要がある
	l_commandQueue->ExecuteCommandLists(_countof(l_list), l_list);
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

bool FWK::Graphics::CommandQueueBase::CreateCommandQueue(const Device& a_device)
{
	const auto& l_device = a_device.GetDevice();

	// デバイスが存在しないなら作成できないのでreturn
	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、コマンドキューの作成が出来ませんでした。");
		return false;
	}

	// コマンドキュー作成時に必要な設定構造体
	// この構造体に「どんな種類のキューを作るか」を設定してからCreateCommandQueueに渡す
	auto l_desc = D3D12_COMMAND_QUEUE_DESC();

	// このコマンドキューで実行するコマンドリストの種類を指定する
	// ※注意 : この値は実行するコマンドリストのTypeと一致している必要がある
	l_desc.Type = k_createCommandListType;

	// コマンドキューの優先度を指定する
	l_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	
	// コマンドキューの追加オプションを指定する
	l_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// このコマンドキューをどのGPUノードで使用するかを指定する
	l_desc.NodeMask = a_device.GetDefaultGPUNodeMask();

	// コマンドキューを作成する関数
	// CreateCommandQueue(コマンドキューの設定内容、
	//					  受け取りたいCOMインターフェース型のID、
	//					  作成結果のポインタを書き込むアドレス);

	auto l_hr = l_device->CreateCommandQueue(&l_desc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::CommandQueueBase::CreateFence(const Device& a_device)
{
	return m_fence.Create(a_device);
}