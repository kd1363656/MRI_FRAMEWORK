#include "CommandQueueBase.h"

FWK::Graphics::CommandQueueBase::CommandQueueBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_createCommandListType(a_createCommandListType),

	m_commandQueue(nullptr)
{}
FWK::Graphics::CommandQueueBase::~CommandQueueBase() = default;

bool FWK::Graphics::CommandQueueBase::Create(const Device& a_device)
{
	if (!CreateCommandQueue(a_device))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::CommandQueueBase::CreateCommandQueue(const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

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
	l_desc.NodeMask = GraphicsManager::GetVALDefaultGPUNodeMask();

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