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
	
	if (!l_backBufferList[l_currentBackBufferIndex].m_backBufferResource)
	{
		assert(false && "バックバッファリソースが無効になっており、BeginFrame処理が行えませんでした。");
		return;
	}

	auto& l_backBufferResource = *l_backBufferList[l_currentBackBufferIndex].m_backBufferResource.Get();

	// リソースの状態遷移(Present -> RenderTarget)
	TransitionResource(a_beforeState, a_afterState, l_backBufferResource);
}
void FWK::Graphics::DirectCommandList::TransitionRenderTargetTexture(const D3D12_RESOURCE_STATES a_afterState, RenderTargetTexture& a_renderTargetTexture) const
{
	const auto& l_gpuResource = a_renderTargetTexture.GetREFGPUResource();

	if (!l_gpuResource.m_resource)
	{
		assert(false && "RenderTargetのリソースが無効のため、状態遷移が出来ませんでした。");
		return;
	}

	const auto l_beforeState = a_renderTargetTexture.GetVALCurrentResourceState();

	// 同じリソース状態なら遷移しない
	if (l_beforeState == a_afterState) { return; }

	TransitionResource(l_beforeState, a_afterState,*l_gpuResource.m_resource.Get());

	a_renderTargetTexture.SetCurrentResourceState(a_afterState);
}
void FWK::Graphics::DirectCommandList::TransitionDepthStencilTexture(const D3D12_RESOURCE_STATES a_afterState, DepthStencilTexture& a_depthStencilTexture) const
{
	const auto& l_gpuResource = a_depthStencilTexture.GetREFGPUResource();

	if (!l_gpuResource.m_resource)
	{
		assert(false && "DepthStencilTextureのリソースが無効のため、状態遷移が出来ませんでした。");
		return;
	}

	const auto l_beforeState = a_depthStencilTexture.GetVALCurrentResourceState();

	// 同じリソース状態なら遷移しない
	if (l_beforeState == a_afterState) { return; }

	TransitionResource(l_beforeState, a_afterState, *l_gpuResource.m_resource.Get());

	a_depthStencilTexture.SetCurrentResourceState(a_afterState);
}

void FWK::Graphics::DirectCommandList::SetupRenderTargetTexture(const RenderTargetTexture& a_renderTargetTexture,
																const RTVDescriptorHeap&   a_rtvDescriptorHeap,
																const DSVDescriptorHeap&   a_dsvDescriptorHeap, 
															    const DepthStencilTexture& a_depthStencilTexture) const
{
	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、RenderTargetTextureの設定が行えませんでした。");
		return;
	}

	if (a_renderTargetTexture.GetVALRTVStorageID() == Constant::k_invalidStorageID)
	{
		assert(false && "RTVStorageIDが無効のため、RenderTargetTextureの設定が行えませんでした。");
		return;
	}

	if (a_depthStencilTexture.GetVALDSVStorageID() == Constant::k_invalidStorageID)
	{
		assert(false && "DSVStorageIDが無効のため、RenderTargetTextureの設定が行えませんでした。");
		return;
	}

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

	// ClearDepthStencilView(クリアするDSV、
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

	// 現在のレンダーターゲットを指定色でクリアする関数
	// ClearRenderTargetView(クリア対象のRTVハンドル、
	//						 クリア色RGBA配列、
	//						 部分クリアする矩形数(0の場合は矩形指定なしとみなし全面クリアとなる),
	//						 矩形配列の先頭アドレス);

	l_directCommandList->ClearRenderTargetView(l_rtvHandle,
											   &a_renderTargetTexture.GetClearColor().x,
											   k_executeClearRectNUM,
											   nullptr);
}

void FWK::Graphics::DirectCommandList::SetupBackBufferRenderTarget(const SwapChain&		  	  a_swapChain, 
																   const RTVDescriptorHeap&   a_rtvDescriptorHeap, 
																   const DSVDescriptorHeap&   a_dsvDescriptorHeap, 
																   const DepthStencilTexture& a_depthStencilTexture) const
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

	if (l_backBuffer.m_rtvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "RTVStorageIDが無効のため、バックバッファの設定が行えませんでした。");
		return;
	}

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

	// 現在のレンダーターゲットを指定色でクリアする関数
	// ClearRenderTargetView(クリア対象のRTVハンドル、
	//						 クリア色RGBA配列、
	//						 部分クリアする矩形数(0の場合は矩形指定なしとみなし前面クリアとなる)、
	//						 矩形配列の先頭アドレス);

	l_directCommandList->ClearRenderTargetView(l_rtvHandle,
											   &Constant::k_renderTargetDefaultClearColor.x, 
											   k_executeClearRectNUM,
											   nullptr);
}

void FWK::Graphics::DirectCommandList::CopyRenderTargetTexture(const RenderTargetTexture& a_renderTargetTexture, const SwapChain& a_swapChain) const
{
	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、RenderTargetTextureをBackBufferへコピーできませんでした。");
		return;
	}

	const auto& l_backBufferList = a_swapChain.GetREFBackBufferList();

	if (l_backBufferList.empty())
	{
		assert(false && "BackBufferListが空のため、RenderTargetTextureをBackBufferへコピーできませんでした。");
		return;
	}

	const auto l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();

	if (l_backBufferList.size() <= l_currentBackBufferIndex)
	{
		assert(false && "現在のBackBufferIndexが範囲外のため、RenderTargetTextureをBackBufferへコピーできませんでした。");
		return;
	}

	const auto& l_backBufferResource = l_backBufferList[l_currentBackBufferIndex].m_backBufferResource;
	const auto& l_sourceResource	 = a_renderTargetTexture.GetREFGPUResource().m_resource;

	if (!l_backBufferResource)
	{
		assert(false && "BackBufferResourceが無効のため、RenderTargetTextureをBackBufferへコピーできませんでした。");
		return;
	}

	if (!l_sourceResource)
	{
		assert(false && "RenderTargetTextureのResourceが無効のため、BackBufferへコピーできませんでした。");
		return;
	}

	// CopyResource(コピー先リソース、
	//				コピー元リソース);
	l_directCommandList->CopyResource(l_backBufferResource.Get(),  l_sourceResource.Get());
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

	if (!l_rootSignature)
	{
		assert(false && "ルートシグネチャが破棄されているため、ルートシグネチャの設定に失敗しました。");
		return;
	}

	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

	const auto& l_d3dRootSignature = l_rootSignature->GetREFRootSignature();

	if (!l_d3dRootSignature)
	{
		assert(false && "ルートシグネチャが作成されておらず、ルートシグネチャの設定が出来ませんでした。");
		return;
	}

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

	if (!l_pipelineState)
	{
		assert(false && "パイプラインステートが破棄されているため、パイプラインステートの設定が出来ませんでした。");
		return;
	}

	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、パイプラインステートの設定が出来ませんでした。");
		return;
	}

	const auto& l_d3dPipelineState = l_pipelineState->GetREFPipelineState();

	if (!l_d3dPipelineState)
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
	l_directCommandList->SetPipelineState(l_d3dPipelineState.Get());

	// 現在セット済みのパイプラインステートとして記録する
	m_currentPipelineState = a_pipelineState;
}

void FWK::Graphics::DirectCommandList::SetupDescriptorHeap(const DescriptorHeapBase& a_descriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、ShaderVisibleなディスクリプタヒープの設定ができませんでした。");
		return;
	}

	const auto& l_descriptorHeap = a_descriptorHeap.FetchVALShaderVisibleDescriptorHeap();

	if (!l_descriptorHeap)
	{
		assert(false && "ShaderVisibleなディスクリプタヒープが作成されておらず、ShaderVisibleなディスクリプタヒープの設定ができませんでした。");
		return;
	}

	ID3D12DescriptorHeap* l_descriptorHeapList[] =
	{
		l_descriptorHeap.Get()
	};

	// シェーダーから参照するDescriptorHeapを設定する
	// SetDescriptorHeap(設定するヒープ数,
	//					 ヒープ配列の先頭アドレス);
	
	l_directCommandList->SetDescriptorHeaps(k_setDescriptorHeapNUM, l_descriptorHeapList);
}

void FWK::Graphics::DirectCommandList::SetupBackBufferRenderTarget(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const
{
	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、バックバッファの設定が行えませんでした。");
		return;
	}

	const auto& l_backBufferList		 = a_swapChain.GetREFBackBufferList			 ();
	const auto  l_currentBackBufferIndex = a_swapChain.FetchVALCurrentBackBufferIndex();

	if (l_currentBackBufferIndex >= static_cast<UINT>(l_backBufferList.size()))
	{
		assert(false && "現在のインデックスがバックバッファリストの範囲外を指し示しており、バックバッファの設定が行えませんでした。");
		return;
	}

	const auto& l_backBuffer = l_backBufferList[l_currentBackBufferIndex];

	if (l_backBuffer.m_rtvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "RTVStorageIDが無効のため、バックバッファの設定が行えませんでした。");
		return;
	}

	// 現在のバックバッファ番号に対応したRTVハンドルを取得する
	const auto& l_rtvHandle = a_rtvDescriptorHeap.FetchVALCPUOnlyCPUHandle(l_backBuffer.m_rtvStorageID);

	// OMステージにレンダーターゲットを設定する関数
	// OMSetRenderTargets(設定するレンダーターゲット数、
	//					  レンダーターゲットディスクリプタ配列の先頭アドレス、
	//					  ディスクリプタが連続配置かどうか、
	//					　深度ステンシルビューのアドレス);

	l_directCommandList->OMSetRenderTargets(k_executeRenderTargetNUM,
								            &l_rtvHandle,
								            true,
								            nullptr);

	// 現在のレンダーターゲットを指定色でクリアする関数
	// ClearRenderTargetView(クリア対象のRTVハンドル、
	//						 クリア色RGBA配列、
	//						 部分クリアする矩形数(0の場合は矩形指定なしとみなし前面クリアとなる)、
	//						 矩形配列の先頭アドレス);

	l_directCommandList->ClearRenderTargetView(l_rtvHandle,
											   &Constant::k_renderTargetDefaultClearColor.x, 
											   k_executeClearRectNUM,
											   nullptr);
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
void FWK::Graphics::DirectCommandList::DispatchFullScreenTriangle() const
{
	const auto& l_directCommandList = GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "ダイレクトコマンドリストが作成されておらず、フルスクリーン三角形の描画が出来ませんでした。");
		return;
	}

	// FinalPresent用MeshShaderは1グループで画面全体の三角形を1枚出力する
	// DispatchMesh(X方向のグループ数、
	//				Y方向のグループ数、
	//				Z方向のグループ数);
	l_directCommandList->DispatchMesh(k_fullScreenTriangleThreadGroupCountX, k_fullScreenTriangleThreadGroupCountY, k_fullScreenTriangleThreadGroupCountZ);
}

void FWK::Graphics::DirectCommandList::ClearCurrentRootSignatureAndPipelineStateCache()
{
	m_currentRootSignature.reset();
	m_currentPipelineState.reset();
}