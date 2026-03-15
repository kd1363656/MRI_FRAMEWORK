#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList() : 
	CommandListBase(D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;

void FWK::Graphics::DirectCommandList::TransitionResource(const Microsoft::WRL::ComPtr<ID3D12Resource2>& a_resource, const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState) const
{
	if (!a_resource)
	{
		assert(false && "リソースが作成されておらず、リソースの遷移ができませんでした。");
		return;
	}

	const auto& l_directCommandList = GetCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

	// D3D12_RESOURCE_BARRIER構造体についての説明(CD3DX12_RESOURCE_BARRIER::Transition内部で使用)
	// Type                   : このバリアがどういうバリアであるかを指定
	// Flags                  : バリアの特別な追加設定
	// Transition.pResource   : 状態を切り替える対象のGPUリソース
	// Transition.StateBefor  : 切り替える前のリソース状態
	// Transition.StateAfter  : 切り替えた後のリソース状態
	// Transition.Subresource : どのサブリソースを遷移対象にするか

	const auto& l_barrier = CD3DX12_RESOURCE_BARRIER::Transition(a_resource.Get(), a_beforeState, a_afterState);

	// リソースバリアを転送
	// ResourceBarrier(送るバリア数、
	//				   バリア情報の戦闘アドレス)

	l_directCommandList->ResourceBarrier(k_sendBarrierNum, &l_barrier);
}