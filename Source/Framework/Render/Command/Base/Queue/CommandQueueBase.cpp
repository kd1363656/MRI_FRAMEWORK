#include "CommandQueueBase.h"

void FWK::Render::CommandQueueBase::Init()
{
	m_commandQueue.Reset();
}
bool FWK::Render::CommandQueueBase::Create(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType)
{
	const auto& l_device = a_hardware.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、コマンドキューの作成ができません。");
		return false;
	}

	// コマンドキューのパラメータを設定
	D3D12_COMMAND_QUEUE_DESC l_desc = {};
	l_desc.Type                     = a_createCommandListType;			   // 作製するコマンドキューの種類、作製するコマンドリストに合わせる必要がある
	l_desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 優先度
	l_desc.Flags				    = D3D12_COMMAND_QUEUE_FLAG_NONE;	   // オプション
	l_desc.NodeMask                 = a_hardware.GetGPUNodeMask();		   // どの"GPU"ノード用のコマンドキューか(単一"GPU"を想定しているので"0")、

	auto l_hr = l_device->CreateCommandQueue(&l_desc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	return true;
}