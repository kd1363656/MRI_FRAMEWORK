#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList() : 
	CommandListBase(Constant::k_directCommandListType)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;

void FWK::Graphics::DirectCommandList::TransitionResource(const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState, const TypeAlias::ComPtr<ID3D12Resource2>& a_resource) const
{
	if (!a_resource)
	{
		assert(false && "リソースが作成されておらず、リソースの遷移ができませんでした。");
		return;
	}

	if (a_beforeState == a_afterState)
	{
		assert(false && "リソースの状態遷移前と後の遷移状態が全く一緒です、リソースの遷移ができませんでした。");
		return;
	}

	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、リソースの遷移が出来ませんでした。");
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
	//				   バリア情報の先頭アドレス)

	l_directCommandList->ResourceBarrier(k_sendBarrierNum, &l_barrier);
}

void FWK::Graphics::DirectCommandList::TransitionRenderTargetResource(const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState, const SwapChain& a_swapChain) const
{
	const auto& l_backBufferList = a_swapChain.GetREFBackBufferList();
	
	if (l_backBufferList.empty())
	{
		assert(false && "バックバッファリストの要素が存在せず、BeginFrame処理が行えませんでした。");
		return;
	}
	
	const auto l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();
	
	if (l_backBufferList.size() <= l_currentBackBufferIndex)
	{
		assert(false && "バックバッファの要素数を超えており、BeginFrame処理が行えませんでした。");
		return;
	}
	
	const auto& l_backBuffer = l_backBufferList[l_currentBackBufferIndex].m_backBufferResourceREF;

	// リソースの状態遷移(Present -> RenderTarget)
	TransitionResource(a_beforeState, a_afterState, l_backBuffer);
}

void FWK::Graphics::DirectCommandList::SetupBackBuffer(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList().Get();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、バックバッファの設定が行えませんでした。");
		return;
	}

	const auto& l_backBufferList         = a_swapChain.GetREFBackBufferList          ();
	const auto  l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();

	if (l_currentBackBufferIndex >= static_cast<UINT>(l_backBufferList.size()))
	{
		assert(false && "現在のインデックスがバックバッファリストの範囲外を指し示しており、バックバッファの設定が行えませんでした。");
		return;
	}

	const auto& l_backBuffer = l_backBufferList[l_currentBackBufferIndex];

	const auto& l_rtvIndex = l_backBuffer.m_rtvIndex;

	if (l_rtvIndex == Constant::k_invalidDescriptorHeapIndex)
	{
		assert(false && "RTVインデックスが無効のため、バックバッファの設定が行えませんでした。");
		return;
	}

	// 現在のバックバッファ番号に対応したRTVハンドルを取得する
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(l_rtvIndex);

	// OMステージにレンダーターゲットを設定する関数
	// OMSetRenderTargets(設定するレンダーターゲット数、
	//					  レンダーターディスクリプタ配列の先頭アドレス、
	//					  ディスクリプタが連続は位置かどうか、
	//					　深度ステンシルビューのアドレス);

	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNum,
								            &l_rtvHandle,
								            true,
								            nullptr);

	// 現在のレンダーターゲットを指定職でクリアする関数
	// ClearRenderTargetView(クリア対象のRTVハンドル、
	//						 クリア色RGBA配列、
	//						 部分クリアする矩形数(0の場合は矩形指定なしとみなし前面クリアとなる)、
	//						 矩形配列の先頭アドレス);

	l_directCommandList->ClearRenderTargetView(l_rtvHandle,
											   k_clearColor, 
											   k_executeClearRectNum,
											   nullptr);
}

void FWK::Graphics::DirectCommandList::SetupRenderArea(const RenderArea& a_renderArea) const
{
	const auto& l_directCommandList = GetREFCommandList().Get();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ビューポートとシザー矩形の設定が出来ませんでした。");
		return;
	}
	
	// Viewportは頂点変換後の座標(NDC -1.0F ~ 1.0F)を、実際の画面上のどこに描くかを決める設定
	// Viewportをコマンドリストへ設定する関数
	// RSSetViewports(設定するViewportの数、
	//				  Viewport構造体のアドレス);

	l_directCommandList->RSSetViewports(k_setViewportNum, &a_renderArea.GetREFViewport());

	// ScissorRectをコマンドリストへ設定する
	// ScissorRectは実際に描画してよいピクセル範囲を制限する四角形
	// RSSetScissorRects(設定するScissorRectの数、
	//					 ScissorRectの先頭アドレス)

	l_directCommandList->RSSetScissorRects(k_setScissorRectNum, &a_renderArea.GetScissorRect());
}
void FWK::Graphics::DirectCommandList::SetupRootSignature(const RootSignature* a_rootSignature) const
{
	if (!a_rootSignature)
	{
		assert(false && "無効なルートシグネチャで、ルートシグネチャの設定ができませんでした。");
		return;
	}

	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

	const auto& l_rootSignature = a_rootSignature->GetREFRootSignature();

	if (!l_rootSignature)
	{
		assert(false && "ルートシグネチャが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

	// コマンドリストにルートシグネチャを設定する関数
	// SetGraphicsRootSignature(描画パイプラインで使用するルートシグネチャのポインタ);

	// ルートシグネチャは、シェーダーにどのリソースをどう渡すかを表す設定情報
	// これを先に設定しておかない、後続の描画で使用するリリースの結び付けルールが決まらない
	l_directCommandList->SetGraphicsRootSignature(l_rootSignature.Get());
}
void FWK::Graphics::DirectCommandList::SetupPipelineState(const PipelineState* a_pipelineState) const
{
	if (!a_pipelineState)
	{
		assert(false && "無効なパイプラインステートで、パイプラインステートの設定が出来ませんでした。");
		return;
	}

	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、パイプラインステートの設定が出来ませんでした。");
		return;
	}

	const auto& l_pipelineState = a_pipelineState->GetREFPipelineState();

	if (!l_pipelineState)
	{
		assert(false && "パイプラインステートが作成されておらず、パイプラインステートの設定が出来ませんでした。");
		return;
	}

	// コマンドリストにパイプラインステートをセットする関数
	// SetPipelineState(パイプラインステートのポインタ)

	// PSO(PipelineStateObject)には、
	// どのシェーダーを使うか、
	// どうラスタライズするか
	// 深度テストを使うか、など
	// 描画パイプラインの重要な設定がまとめて入っている
	l_directCommandList->SetPipelineState(l_pipelineState.Get());
}

void FWK::Graphics::DirectCommandList::DispatchMesh(const UINT a_threadCountGroupX, const UINT a_threadCountGroupY, const UINT a_threadCountGroupZ) const
{
	const auto& l_directCommandList = GetREFCommandList().Get();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、DispatchMesh処理が出来ませんでした。");
		return;
	}

	// メッシュシェーダーを実行するための関数
	// DispatchMesh(X方向のグループ数、
	//				Y方向のグループ数、
	//				Z方向のグループ数);

	l_directCommandList->DispatchMesh(a_threadCountGroupX, a_threadCountGroupY, a_threadCountGroupZ);
}