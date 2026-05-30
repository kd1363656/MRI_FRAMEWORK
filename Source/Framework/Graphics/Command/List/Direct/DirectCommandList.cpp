#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList() : 
	CommandListBase(Constant::k_createCommandListTypeDirect)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;

void FWK::Graphics::DirectCommandList::Reset(const CommandAllocatorBase& a_commandAllocator)
{
	CommandListBase::Reset(a_commandAllocator);

	// パイプラインステートとルートシグネチャをセットするかどうかを判断するキャッシュのクリア
	ClearCurrentRootSignatureAndPipelineStateCache();
}

void FWK::Graphics::DirectCommandList::TransitionResource(const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState, ID3D12Resource2& a_resource) const
{
	FWK_ASSERT_RETURN_IF(a_beforeState == a_afterState, "リソースの状態遷移前と後の遷移状態が全く一緒です、リソースの遷移ができませんでした。")

	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、リソースの遷移が出来ませんでした。")

	// D3D12_RESOURCE_BARRIER構造体についての説明(CD3DX12_RESOURCE_BARRIER::Transition内部で使用)
	// Type                   : このバリアがどういうバリアであるかを指定
	// Flags                  : バリアの特別な追加設定
	// Transition.pResource   : 状態を切り替える対象のGPUリソース
	// Transition.StateBefore : 切り替える前のリソース状態
	// Transition.StateAfter  : 切り替えた後のリソース状態
	// Transition.Subresource : どのサブリソースを遷移対象にするか

	const auto& l_barrier = CD3DX12_RESOURCE_BARRIER::Transition(&a_resource, a_beforeState, a_afterState);

	// リソースバリアを転送
	// ResourceBarrier(送るバリア数、
	//				   バリア情報の先頭アドレス)
	l_directCommandList->ResourceBarrier(k_sendBarrierNUM, &l_barrier);
}

void FWK::Graphics::DirectCommandList::TransitionRenderTargetResource(const SwapChain& a_swapChain, const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState) const
{
	const auto& l_backBufferList = a_swapChain.GetREFBackBufferList();
	
	FWK_ASSERT_RETURN_IF(l_backBufferList.empty(), "バックバッファリストの要素が存在せず、BeginFrame処理が行えませんでした。")

	const auto l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();
	
	FWK_ASSERT_RETURN_IF(l_backBufferList.size() <= l_currentBackBufferIndex,			   "バックバッファの要素数を超えており、BeginFrame処理が行えませんでした。")
	FWK_ASSERT_RETURN_IF(!l_backBufferList[l_currentBackBufferIndex].m_backBufferResource, "バックバッファリソースが無効になっており、BeginFrame処理が行えませんでした。")

	auto& l_backBufferResource = *l_backBufferList[l_currentBackBufferIndex].m_backBufferResource.Get();

	// リソースの状態遷移(Present -> RenderTarget)
	TransitionResource(a_beforeState, a_afterState, l_backBufferResource);
}
void FWK::Graphics::DirectCommandList::TransitionRenderTargetTexture(const D3D12_RESOURCE_STATES a_afterState, RenderTargetTexture& a_renderTargetTexture) const
{
	const auto& l_gpuResource = a_renderTargetTexture.GetREFGPUResource();

	FWK_ASSERT_RETURN_IF(!l_gpuResource.m_resource, "RenderTargetのリソースが無効のため、状態遷移が出来ませんでした。")

	const auto l_beforeState = a_renderTargetTexture.GetVALCurrentResourceState();

	// 同じリソース状態なら遷移しない
	if (l_beforeState == a_afterState) { return; }

	TransitionResource(l_beforeState, a_afterState,*l_gpuResource.m_resource.Get());

	a_renderTargetTexture.SetCurrentResourceState(a_afterState);
}
void FWK::Graphics::DirectCommandList::TransitionDepthStencilTexture(const D3D12_RESOURCE_STATES a_afterState, DepthStencilTexture& a_depthStencilTexture) const
{
	const auto& l_gpuResource = a_depthStencilTexture.GetREFGPUResource();

	FWK_ASSERT_RETURN_IF(!l_gpuResource.m_resource, "DepthStencilTextureのリソースが無効のため、状態遷移が出来ませんでした。")

	const auto l_beforeState = a_depthStencilTexture.GetVALCurrentResourceState();

	// 同じリソース状態なら遷移しない
	if (l_beforeState == a_afterState) { return; }

	TransitionResource(l_beforeState, a_afterState, *l_gpuResource.m_resource.Get());

	a_depthStencilTexture.SetCurrentResourceState(a_afterState);
}

void FWK::Graphics::DirectCommandList::SetupRenderTargetTexture(const RenderTargetTexture& a_renderTargetTexture, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList,														 "DirectCommandListが無効のため、RenderTargetTextureの設定が行えませんでした。")
	FWK_ASSERT_RETURN_IF(a_renderTargetTexture.GetVALRTVStorageID() == Constant::k_invalidStorageID, "RTVStorageIDが無効のため、RenderTargetTextureの設定が行えませんでした。")

	// RenderTargetTexture用RTVハンドルを取得
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_renderTargetTexture.GetVALRTVStorageID());

	// OMステージにレンダーターゲットを設定する関数
	// OMSetRenderTargets(設定するレンダーターゲット数、
	//					  レンダーターゲットディスクリプタ配列の先頭アドレス、
	//					  ディスクリプタ連続配列かどうか、
	//					  深度ステンシルビューのアドレス);
	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNUM,
											&l_rtvHandle,
											true,
											nullptr);
}

void FWK::Graphics::DirectCommandList::SetupRenderTargetTexture(const RenderTargetTexture& a_renderTargetTexture,
																const RTVDescriptorHeap&   a_rtvDescriptorHeap,
																const DSVDescriptorHeap&   a_dsvDescriptorHeap, 
															    const DepthStencilTexture& a_depthStencilTexture) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList,														 "ダイレクトコマンドリストが作成されておらず、RenderTargetTextureの設定が行えませんでした。")
	FWK_ASSERT_RETURN_IF(a_renderTargetTexture.GetVALRTVStorageID() == Constant::k_invalidStorageID, "RTVStorageIDが無効のため、RenderTargetTextureの設定が行えませんでした。")
	FWK_ASSERT_RETURN_IF(a_depthStencilTexture.GetVALDSVStorageID() == Constant::k_invalidStorageID, "DSVStorageIDが無効のため、RenderTargetTextureの設定が行えませんでした。")

	// RenderTargetTexture用RTVハンドルとDepthStencilTexture用DSVハンドルを取得
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_renderTargetTexture.GetVALRTVStorageID());
	const auto& l_dsvHandle = a_dsvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_depthStencilTexture.GetVALDSVStorageID());

	// OMステージにレンダーターゲットを設定する関数
	// OMSetRenderTargets(設定するレンダーターゲット数、
	//					  レンダーターゲットディスクリプタ配列の先頭アドレス、
	//					  ディスクリプタ連続配置かどうか、
	//					  深度ステンシルビューのアドレス);
	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNUM,
											&l_rtvHandle,
											true,
											&l_dsvHandle);
}

void FWK::Graphics::DirectCommandList::SetupBackBufferRenderTarget(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、バックバッファの設定が行えませんでした。")

	const auto& l_backBufferList		 = a_swapChain.GetREFBackBufferList			 ();
	const auto  l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();

	FWK_ASSERT_RETURN_IF(l_currentBackBufferIndex >= static_cast<UINT>(l_backBufferList.size()), "現在のインデックスがバックバッファリストの範囲外を指し示しており、バックバッファの設定が行えませんでした。")

	const auto& l_backBuffer = l_backBufferList[l_currentBackBufferIndex];

	FWK_ASSERT_RETURN_IF(l_backBuffer.m_rtvStorageID == Constant::k_invalidStorageID, "RTVStorageIDが無効のため、バックバッファの設定が行えませんでした。")

	// 現在のバックバッファ番号に対応したRTVハンドルを取得する
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(l_backBuffer.m_rtvStorageID);

	// OMステージにバックバッファをレンダーターゲットとして設定する関数
	// OMSetRenderTargets(設定するレンダーターゲット数、
	//					  レンダーターゲットディスクリプタ配列の先頭アドレス、
	//					  ディスクリプタが連続配置かどうか、
	//					  深度ステンシルビューのアドレス);
	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNUM,
											&l_rtvHandle,
											true,
											nullptr);
}
void FWK::Graphics::DirectCommandList::SetupBackBufferRenderTarget(const SwapChain&		  	  a_swapChain, 
																   const RTVDescriptorHeap&   a_rtvDescriptorHeap, 
																   const DSVDescriptorHeap&   a_dsvDescriptorHeap, 
																   const DepthStencilTexture& a_depthStencilTexture) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、バックバッファの設定が行えませんでした。")

	const auto& l_backBufferList         = a_swapChain.GetREFBackBufferList          ();
	const auto  l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();

	FWK_ASSERT_RETURN_IF(l_currentBackBufferIndex >= static_cast<UINT>(l_backBufferList.size()), "現在のインデックスがバックバッファリストの範囲外を指し示しており、バックバッファの設定が行えませんでした。")

	const auto& l_backBuffer = l_backBufferList[l_currentBackBufferIndex];

	FWK_ASSERT_RETURN_IF(l_backBuffer.m_rtvStorageID == Constant::k_invalidStorageID,				 "RTVStorageIDが無効のため、バックバッファの設定が行えませんでした。")
	FWK_ASSERT_RETURN_IF(a_depthStencilTexture.GetVALDSVStorageID() == Constant::k_invalidStorageID, "DSVStorageIDが無効のため、バックバッファの設定が行えませんでした。")

	// 現在のバックバッファ番号に対応したRTVハンドルを取得する
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(l_backBuffer.m_rtvStorageID);
	const auto& l_dsvHandle = a_dsvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_depthStencilTexture.GetVALDSVStorageID());

	// OMステージにレンダーターゲットを設定する関数
	// OMSetRenderTargets(設定するレンダーターゲット数、
	//					  レンダーターゲットディスクリプタ配列の先頭アドレス、
	//					  ディスクリプタが連続配置かどうか、
	//					　深度ステンシルビューのアドレス);

	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNUM,
								            &l_rtvHandle,
								            true,
								            &l_dsvHandle);
}

void FWK::Graphics::DirectCommandList::ClearRenderTargetTexture(const RenderTargetTexture& a_renderTargetTexture, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList,														 "ダイレクトコマンドリストが作成されておらず、RenderTargetTextureのクリアが行えませんでした。")
	FWK_ASSERT_RETURN_IF(a_renderTargetTexture.GetVALRTVStorageID() == Constant::k_invalidStorageID, "RTVStorageIDが無効のため、RenderTargetTextureのクリアが行えませんでした。")

	// RenderTargetTexture用RTVハンドル
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_renderTargetTexture.GetVALRTVStorageID());

	// 現在のレンダーターゲットを指定色でクリアする関数
	// ClearRenderTargetView(クリア対象のRTVハンドル、
	//						 クリア色RGBA配列、
	//						 部分クリアする矩形数(0の場合は矩形指定なしとみなし全面クリア)、
	//						 矩形配列の先頭アドレス);
	l_directCommandList->ClearRenderTargetView(l_rtvHandle,
											   &a_renderTargetTexture.GetClearColor().x,
											   k_executeClearRectNUM,
											   nullptr);
}
void FWK::Graphics::DirectCommandList::ClearDepthStencilTexture(const DepthStencilTexture& a_depthStencilTexture, const DSVDescriptorHeap& a_dsvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList,														 "ダイレクトコマンドリストが作成されておらず、DepthStencilTextureのクリアが行えませんでした。")
	FWK_ASSERT_RETURN_IF(a_depthStencilTexture.GetVALDSVStorageID() == Constant::k_invalidStorageID, "DSVStorageIDが無効のため、DepthStencilTextureのクリアが行えませんでした。")

	// DepthStencilTexture用DSVハンドルを取得する
	const auto& l_dsvHandle = a_dsvDescriptorHeap.FetchVALCPUOnlyCPUHandle(a_depthStencilTexture.GetVALDSVStorageID());

	// ClearDepthStencilView(クリアするDSV,
	//						 クリア対象フラグ、
	//						 深度クリア値、
	//						 ステンシルクリア値、
	//						 クリア範囲数、
	//						 クリア範囲);
	l_directCommandList->ClearDepthStencilView(l_dsvHandle,
											   D3D12_CLEAR_FLAG_DEPTH,
											   Constant::k_defaultDepthClearValue,
											   Constant::k_defaultStencilClearValue,
											   k_executeClearRectNUM,
											   nullptr);
}
void FWK::Graphics::DirectCommandList::ClearBackBufferRenderTarget(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、バックバッファのクリアが行えませんでした。")

	const auto& l_backBufferList		 = a_swapChain.GetREFBackBufferList			 ();
	const auto  l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();

	FWK_ASSERT_RETURN_IF(l_currentBackBufferIndex >= l_backBufferList.size(), "RTVStorageIDが無効のため、バックバッファのクリアが行えませんでした。")

	const auto& l_backBuffer = l_backBufferList[l_currentBackBufferIndex];

	FWK_ASSERT_RETURN_IF(l_backBuffer.m_rtvStorageID == Constant::k_invalidStorageID, "現在のインデックスがバックバッファリストの範囲外を指し示しており、バックバッファのクリアが行えませんでした。")

	// 現在のバックバッファ番号に対応したRTVハンドルを取得する
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(l_backBuffer.m_rtvStorageID);

	// ClearRenderTargetView(クリア対象のRTVハンドル、
	//						 クリア色RGBA配列、
	//						 部分クリアする矩形数(0の場合は矩形指定なしとみなし全面クリアとなる)、
	//					     矩形配列の先頭アドレス);
	l_directCommandList->ClearRenderTargetView(l_rtvHandle,
											   &Constant::k_renderTargetDefaultClearColor.x,
											   k_executeClearRectNUM,
											   nullptr);
}

void FWK::Graphics::DirectCommandList::SetupRenderArea(const RenderArea& a_renderArea) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、ビューポートとシザー矩形の設定が出来ませんでした。")

	// Viewportは頂点変換後の座標(NDC -1.0F ~ 1.0F)を、実際の画面上のどこに描くかを決める設定
	// Viewportをコマンドリストへ設定する関数
	// RSSetViewports(設定するViewportの数、
	//				  Viewport構造体のアドレス);
	l_directCommandList->RSSetViewports(k_setViewportNUM, &a_renderArea.GetREFViewport());

	// ScissorRectをコマンドリストへ設定する
	// ScissorRectは実際に描画してよいピクセル範囲を制限する四角形
	// RSSetScissorRects(設定するScissorRectの数、
	//					 ScissorRectの先頭アドレス)
	l_directCommandList->RSSetScissorRects(k_setScissorRectNUM, &a_renderArea.GetREFScissorRECT());
}
void FWK::Graphics::DirectCommandList::SetupRootSignature(const std::weak_ptr<RootSignature>& a_rootSignature)
{
	// 現在セットしようとしているルートシグネチャと前回セットしたルートシグネチャが同一
	// であるならばセットしない
	if (IsSameWeakOwner(m_currentRootSignature, a_rootSignature)) { return; }

	const auto l_rootSignature = a_rootSignature.lock();

	FWK_ASSERT_RETURN_IF(!l_rootSignature, "ルートシグネチャが破棄されているため、ルートシグネチャの設定に失敗しました。")

	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、ルートシグネチャの設定が出来ませんでした。")

	const auto& l_d3dRootSignature = l_rootSignature->GetREFRootSignature();

	FWK_ASSERT_RETURN_IF(!l_d3dRootSignature, "ルートシグネチャが作成されておらず、ルートシグネチャの設定が出来ませんでした。")

	// コマンドリストにルートシグネチャを設定する関数
	// SetGraphicsRootSignature(描画パイプラインで使用するルートシグネチャのポインタ);
	// ルートシグネチャは、シェーダーにどのリソースをどう渡すかを表す設定情報
	// これを先に設定しておかないと、後続の描画で使用するリソースの結び付けルールが決まらない
	l_directCommandList->SetGraphicsRootSignature(l_d3dRootSignature.Get());

	// 現在セット済みのルートシグネチャとして記録する
	m_currentRootSignature = a_rootSignature;
}
void FWK::Graphics::DirectCommandList::SetupPipelineState(const std::weak_ptr<PipelineState>& a_pipelineState)
{
	// 現在セットしようとしているパイプラインステートと前回セットしたパイプラインステートが同一
	// であるならばセットしない
	if (IsSameWeakOwner(m_currentPipelineState, a_pipelineState)) { return; }

	const auto& l_pipelineState = a_pipelineState.lock();

	FWK_ASSERT_RETURN_IF(!l_pipelineState, "パイプラインステートが破棄されているため、パイプラインステートの設定が出来ませんでした。")

	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、パイプラインステートの設定が出来ませんでした。")

	const auto& l_d3dPipelineState = l_pipelineState->GetREFPipelineState();

	FWK_ASSERT_RETURN_IF(!l_d3dPipelineState, "パイプラインステートが作成されておらず、パイプラインステートの設定が出来ませんでした。")

	// コマンドリストにパイプラインステートをセットする関数
	// SetPipelineState(パイプラインステートのポインタ)
	// PSO(PipelineStateObject)には、
	// どのシェーダーを使うか、
	// どうラスタライズするか
	// 深度テストを使うか、など
	// 描画パイプラインの重要な設定がまとめて入っている
	l_directCommandList->SetPipelineState(l_d3dPipelineState.Get());

	// 現在セット済みのパイプラインステートとして記録する
	m_currentPipelineState = a_pipelineState;
}

void FWK::Graphics::DirectCommandList::SetupDescriptorHeap(const DescriptorHeapBase& a_descriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、ShaderVisibleなディスクリプタヒープの設定ができませんでした。")

	const auto& l_descriptorHeap = a_descriptorHeap.FetchVALShaderVisibleDescriptorHeap();

	FWK_ASSERT_RETURN_IF(!l_descriptorHeap, "ShaderVisibleなディスクリプタヒープが作成されておらず、ShaderVisibleなディスクリプタヒープの設定ができませんでした。")
	
	ID3D12DescriptorHeap* l_descriptorHeapList[] =
	{
		l_descriptorHeap.Get()
	};

	// シェーダーから参照するDescriptorHeapを設定する
	// SetDescriptorHeap(設定するヒープ数,
	//					 ヒープ配列の先頭アドレス);
	l_directCommandList->SetDescriptorHeaps(k_setDescriptorHeapNUM, l_descriptorHeapList);
}

void FWK::Graphics::DirectCommandList::DispatchMesh(const UINT a_threadCountGroupX, const UINT a_threadCountGroupY, const UINT a_threadCountGroupZ) const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、DispatchMesh処理が出来ませんでした。")

	// メッシュシェーダーを実行するための関数
	// DispatchMesh(X方向のグループ数、
	//				Y方向のグループ数、
	//				Z方向のグループ数);
	l_directCommandList->DispatchMesh(a_threadCountGroupX, a_threadCountGroupY, a_threadCountGroupZ);
}
void FWK::Graphics::DirectCommandList::DispatchFullScreen() const
{
	const auto& l_directCommandList = GetREFCommandList();

	FWK_ASSERT_RETURN_IF(!l_directCommandList, "ダイレクトコマンドリストが作成されておらず、フルスクリーン三角形の描画が出来ませんでした。")

	// FinalPresent用MeshShaderは1グループで画面全体の三角形を1枚出力する
	// DispatchMesh(X方向のグループ数、
	//				Y方向のグループ数、
	//				Z方向のグループ数);
	l_directCommandList->DispatchMesh(k_fullScreenThreadGroupCountX, k_fullScreenThreadGroupCountY, k_fullScreenThreadGroupCountZ);
}

void FWK::Graphics::DirectCommandList::ClearCurrentRootSignatureAndPipelineStateCache()
{
	m_currentRootSignature.reset();
	m_currentPipelineState.reset();
}