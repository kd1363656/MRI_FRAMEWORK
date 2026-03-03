#include "DirectCommandQueueh.h"

FWK::Graphics::DirectCommandQueue::DirectCommandQueue(const Device& a_device) : 
	CommandQueueBase(a_device, D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandQueue::~DirectCommandQueue() = default;

void FWK::Graphics::DirectCommandQueue::ExecuteCommandLists(const CommandListBase& a_commandList) const
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