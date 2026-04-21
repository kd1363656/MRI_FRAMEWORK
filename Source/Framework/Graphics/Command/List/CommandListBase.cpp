#include "CommandListBase.h"

FWK::Graphics::CommandListBase::CommandListBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType) :
	k_createCommandListType(a_createCommandListType),

	m_commandList(nullptr)
{}
FWK::Graphics::CommandListBase::~CommandListBase() = default;

bool FWK::Graphics::CommandListBase::Create(const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスの作成に失敗しており、コマンドリストの作成に失敗しました。");
		return false;
	}

	// 閉じた状態のコマンドリストを作成する関数(命令を書き込む前にResetして記録可能状態にする必要がある)
	// CreateCommandList(このコマンドリストを作成するGPUノード指定値、
	//                   このコマンドリストの種類、
	//					 コマンドリスト作成時の追加オプション、
	//					 受け取りたいCOMインターフェース型のID、
	//					 作成結果のポインタを書き込むアドレス);

	auto l_hr = l_device->CreateCommandList1(GraphicsManager::GetDefaultGPUNodeMask(),
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

void FWK::Graphics::CommandListBase::Close() const
{
	if (!m_commandList)
	{
		assert(false && "コマンドリストの作成に失敗しており、コマンドリストのクローズが出来ませんでした。");
		return;
	}

	// コマンドリストへの命令規則を終了するクラス
	// ※注意 : もしCloseをしなければコマンドキューのExecute処理を行うことができない
	// Close();

	m_commandList->Close();
}