#include "RenderTargetTexture.h"

void FWK::Graphics::RenderTargetTexture::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_renderTargetTextureJsonConverter.Deserialize(a_rootJson, *this);
}
void FWK::Graphics::RenderTargetTexture::ApplyWindowSizeIfNeed(const Struct::WindowCONFIG& a_windowConfig)
{
	// ウィンドウサイズに合わせないレンダーターゲットならreturn;
	if (!m_isUseWindowSize) { return; }

	m_width  = a_windowConfig.m_clientSize.m_width;
	m_height = a_windowConfig.m_clientSize.m_height;
}

bool FWK::Graphics::RenderTargetTexture::Create(const Device&							 a_device,
												const GPUMemoryAllocator&				 a_gpuMemoryAllocator, 		
													  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool, 
													  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	// D3D12_CLEAR_VALUEについて
	// Format   : クリア対象リソースのフォーマット
	// Color[0] : Rのクリア値
	// Color[1] : Gのクリア値
	// Color[2] : Bのクリア値
	// Color[3] : Aのクリア値
	D3D12_CLEAR_VALUE l_clearValue = {};

	l_clearValue.Format = m_format;
	l_clearValue.Color[k_clearValueIndexR] = m_clearColor.x;
	l_clearValue.Color[k_clearValueIndexG] = m_clearColor.y;
	l_clearValue.Color[k_clearValueIndexB] = m_clearColor.z;
	l_clearValue.Color[k_clearValueIndexA] = m_clearColor.w;

	// D3D12_RESOURCE_DESCについて
	// Tex2D(フォーマット、
	//		 幅、
	//	     高さ、
	//		 配列数、
	//		 MIP数、
	//		 サンプル数、
	//		 サンプル品質、
	//		 リソースフラグ);
	const auto& l_resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_format,
															  m_width,
															  m_height,
															  Constant::k_renderTextureDefaultArraySize,
															  Constant::k_renderTextureDefaultMIPLevels,
															  Constant::k_renderTextureDefaultSampleCount,
															  Constant::k_renderTextureDefaultSampleQuality,
															  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		
	FWK_ASSERT_RETURN_VALUE_IF(!a_gpuMemoryAllocator.CreateTextureResource(l_resourceDesc,
																		   &l_clearValue,
																		   D3D12_RESOURCE_STATE_RENDER_TARGET,
																		   m_gpuResource),
																		   "RenderTargetTexture用TextureResourceの作成に失敗しました。",
																		   false)

	m_currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	FWK_ASSERT_RETURN_VALUE_IF(!CreateRenderTargetView  (a_device, a_rtvDescriptorPool), "RenderTargetTexture用RTVの作成に失敗しました。", false)
	FWK_ASSERT_RETURN_VALUE_IF(!CreateShaderResourceView(a_device, a_srvDescriptorPool), "RenderTargetTexture用SRVの作成に失敗しました。", false)

	return true;
}

nlohmann::json FWK::Graphics::RenderTargetTexture::Serialize() const
{
	return m_renderTargetTextureJsonConverter.Serialize(*this);
}

bool FWK::Graphics::RenderTargetTexture::Resize(const Device&							 a_device, 
												const GPUMemoryAllocator&				 a_gpuMemoryAllocator, 
												const Struct::ClientSize&				 a_clientSize, 
												const UINT64&							 a_retiredFenceValue, 
													  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
													  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
													  DeferredResourceReleaseQueue&      a_deferredReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!IsValidTextureSize(a_clientSize), "RenderTargetTextureのリサイズ後サイズが無効です。", false)

	if (IsSameTextureSize(a_clientSize)) { return true; }

	FWK_ASSERT_RETURN_VALUE_IF(!IsValidCurrentResourceForDeferredRelease(a_retiredFenceValue), "現在のRenderTargetTextureを遅延解放できない状態です。", false)

	RenderTargetTexture l_newRenderTargetTexture = {};

	l_newRenderTargetTexture.SetClearColor     (m_clearColor);
	l_newRenderTargetTexture.SetFormat         (m_format);
	l_newRenderTargetTexture.SetWidth	       (a_clientSize.m_width);
	l_newRenderTargetTexture.SetHeight	       (a_clientSize.m_height);
	l_newRenderTargetTexture.SetIsUseWindowSize(m_isUseWindowSize);

	// 先にあたらしいRenderTargetTextureを作成。
	// 古いリソースを先にQueueへ写してから新規作成に失敗すると、
	// このインスタンが有効な描画先を失うため
	FWK_ASSERT_RETURN_VALUE_IF(!l_newRenderTargetTexture.Create(a_device,
							   a_gpuMemoryAllocator,
							   a_rtvDescriptorPool,
							   a_srvDescriptorPool),
							   "リサイズ後のRenderTargetTexture作成に失敗しました。",
							   false)

	// 新しいRenderTargetTextureの作成に成功した後で、古いリソースをDeferredReleaseへ渡す
	// Queue内のComPTRが古いGPUResourceを保持するため、
	// GPUが使い終わるまで古いリソースの寿命を延ばせる
	FWK_ASSERT_RETURN_VALUE_IF(!PushCurrentResourceForDeferredRelease(a_retiredFenceValue, a_deferredReleaseQueue), "古いRenderTargetTextureの遅延解放Queue登録に失敗しました。", false)

	// 古いリソースをQueueへ渡せたので、新しリソースを現在のTextureとして採用する
	*this = std::move(l_newRenderTargetTexture);

	return true;
}

bool FWK::Graphics::RenderTargetTexture::IsValidTextureSize(const Struct::ClientSize& a_clientSize) const
{
	if (a_clientSize.m_width  == Constant::k_invalidTextureWidth)  { return false; }
	if (a_clientSize.m_height == Constant::k_invalidTextureHeight) { return false; }

	return true;
}

bool FWK::Graphics::RenderTargetTexture::IsSameTextureSize(const Struct::ClientSize& a_clientSize) const
{
	if (m_width  != a_clientSize.m_width)  { return false; }
	if (m_height != a_clientSize.m_height) { return false; }

	return true;
}

bool FWK::Graphics::RenderTargetTexture::IsValidCurrentResourceForDeferredRelease(const UINT64& a_retiredFenceValue) const
{
	if (!m_gpuResource.m_resource)							 { return false; }
	if (m_rtvStorageID      == Constant::k_invalidStorageID) { return false; }
	if (m_srvStorageID      == Constant::k_invalidStorageID) { return false; }
	if (a_retiredFenceValue == Constant::k_unusedFenceValue) { return false; }

	return true;
}

bool FWK::Graphics::RenderTargetTexture::PushCurrentResourceForDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!IsValidCurrentResourceForDeferredRelease(a_retiredFenceValue), "RenderTargetTextureが無効なため、遅延解放Queueへ登録できません。", false)

	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(m_gpuResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_rtvDescriptorIndexReleaseRecord = {};

	l_rtvDescriptorIndexReleaseRecord.m_storageID		  = m_rtvStorageID;
	l_rtvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_srvDescriptorIndexReleaseRecord = {};

	l_srvDescriptorIndexReleaseRecord.m_storageID		  = m_srvStorageID;
	l_srvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushGPUResourceRecord(std::move(l_gpuResourceReleaseRecord)),		 "RenderTargetTextureのGPUResourceを遅延解放Queueへ登録できませんでした。",		   false)
	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushRTVDescriptorIndex(std::move(l_rtvDescriptorIndexReleaseRecord)), "RenderTargetTextureのRTVDescriptorIndexを遅延解放Queueへ登録できませんでした。", false)
	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushSRVDescriptorIndex(std::move(l_srvDescriptorIndexReleaseRecord)), "RenderTargetTextureのSRVDescriptorIndexを遅延解放Queueへ登録できませんでした。", false)

	// 二重解放を防ぐため、Queueへ渡したDescriptorIndexは無効化します。
	m_rtvStorageID = Constant::k_invalidStorageID;
	m_srvStorageID = Constant::k_invalidStorageID;

	return true;
}

bool FWK::Graphics::RenderTargetTexture::CreateRenderTargetView(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool)
{
	const auto& l_device = a_device.GetREFDevice();

	FWK_ASSERT_RETURN_VALUE_IF(!l_device, "デバイスが作成されておらず、RenderTargetTexture用のRTVの作成に失敗しました。", false)

	const auto l_rtvStorageID = a_rtvDescriptorPool.Allocate();

	FWK_ASSERT_RETURN_VALUE_IF(l_rtvStorageID == Constant::k_invalidStorageID, "RTVStorageIDの確保に失敗しました。", false)

	// D3D12_RENDER_TARGET_VIEW_DESCについて
	// Format		 : RTVとしてみるときのフォーマット
	// ViewDimension : 2DTextureとしてRTVを作成する
	D3D12_RENDER_TARGET_VIEW_DESC l_rtvDesc = {};

	l_rtvDesc.Format		= m_format;
	l_rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	const auto l_rtvHandle = a_rtvDescriptorPool.FetchVALCPUOnlyCPUHandle(l_rtvStorageID);

	// CreateRenderTargetView(RTVを作りたい対象リソース、
	//						  RTV設定、
	//						  RTVを書き込むCPUディスクリプタハンドル);
	l_device->CreateRenderTargetView(m_gpuResource.m_resource.Get(), &l_rtvDesc, l_rtvHandle);

	m_rtvStorageID = l_rtvStorageID;

	return true;
}

bool FWK::Graphics::RenderTargetTexture::CreateShaderResourceView(const Device& a_device, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool)
{
	const auto& l_device = a_device.GetREFDevice();

	FWK_ASSERT_RETURN_VALUE_IF(!l_device, "デバイスが作成されておらず、RenderTargetTexture用のSRVの作成に失敗しました。", false)

	const auto l_srvStorageID = a_srvDescriptorPool.Allocate();

	FWK_ASSERT_RETURN_VALUE_IF(l_srvStorageID == Constant::k_invalidStorageID, "SRVStorageIDの確保に失敗しました。", false)

	// D3D12_SHADER_RESOURCE_VIEW_DESCについて
	// Shader4ComponentMapping : Shader側でRGBA成分をどう読むか
	// Format                  : SRVとしてみるときのフォーマット
	// ViewDimension           : 2DTextureとしてSRVを作成する
	D3D12_SHADER_RESOURCE_VIEW_DESC l_srvDesc = {};

	l_srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	l_srvDesc.Format				  = m_format;
	l_srvDesc.ViewDimension			  = D3D12_SRV_DIMENSION_TEXTURE2D;

	// D3D12_TEX2D_SRVについて
	// MostDetailedMIP     : 読み始めるMIP番号
	// MIPLevels	       : 読めるMIP数
	// PlaneSlice          : 通常カラーTextureなので0
	// ResourceMINLODCLAMP : 最小LOD制限
	l_srvDesc.Texture2D.MostDetailedMip     = k_textureSRVMostDetailedMIP;
	l_srvDesc.Texture2D.MipLevels           = Constant::k_renderTextureDefaultMIPLevels;
	l_srvDesc.Texture2D.PlaneSlice		    = k_textureSRVPlaneSlice;
	l_srvDesc.Texture2D.ResourceMinLODClamp = k_textureSRVResourceMINLODClamp;

	const auto l_cpuOnlyCPUHandle = a_srvDescriptorPool.FetchVALCPUOnlyCPUHandle(l_srvStorageID);

	// CreateShaderResourceView(SRVを作りたい対象リソース、
	//							SRV設定、
	//							SRVを書き込むCPUディスクリプタハンドル);

	l_device->CreateShaderResourceView(m_gpuResource.m_resource.Get(), &l_srvDesc, l_cpuOnlyCPUHandle);

	if (!a_srvDescriptorPool.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(l_srvStorageID, a_device))
	{
		a_srvDescriptorPool.Release(l_srvStorageID);

		FWK_ASSERT_RETURN_VALUE("CPUOnlyからShaderVisibleSRVへのコピーに失敗したため、RenderTargetTexture用SRVの作成に失敗しました。", false)
	}

	m_srvStorageID = l_srvStorageID;

	return true;
}