#include "DepthStencilTexture.h"

void FWK::Graphics::DepthStencilTexture::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_depthStencilTextureJsonConverter.Deserialize(a_rootJson, *this);
}
void FWK::Graphics::DepthStencilTexture::ApplyWindowSizeIfNeed(const Struct::WindowCONFIG& a_windowConfig)
{
	// ウィンドウサイズに合わせないデプスステンシルならreturn;
	if (!m_isUseWindowSize) { return; }

	m_width  = a_windowConfig.m_clientSize.m_width;
	m_height = a_windowConfig.m_clientSize.m_height;
}

bool FWK::Graphics::DepthStencilTexture::Create(const Graphics::Device& a_device, const GPUMemoryAllocator& a_gpuMemoryAllocator, DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	const auto& l_device = a_device.GetREFDevice();

	FWK_ASSERT_RETURN_VALUE_IF(!l_device, "デバイスが作成されておらず、DepthStencilTextureの作成に失敗しました。", false)

	const auto l_dsvStorageID = a_dsvDescriptorPool.Allocate();

	FWK_ASSERT_RETURN_VALUE_IF(l_dsvStorageID == Constant::k_invalidStorageID, "DSVStorageIDの確認に失敗しました。", false)

	// D3D12_CLEAR_VALUEについて
	// Format			    : クリア対象リソースのフォーマット
	// DepthStencil.Depth   : 深度バッファをクリアする値
	// DepthStencil.Stencil : ステンシルバッファをクリアする値
	D3D12_CLEAR_VALUE l_clearValue = {};
	
	l_clearValue.Format				  = m_format;
	l_clearValue.DepthStencil.Depth   = Constant::k_defaultDepthClearValue;
	l_clearValue.DepthStencil.Stencil = Constant::k_defaultStencilClearValue;

	// 書き込み用深度テクスチャの作成
	// D3D12_RESOURCE_DESCについて
	// Tex2D(フォーマット、
	//		 幅、
	//		 高さ、
	//		 配列数、
	//		 Mip数、
	//		 サンプル数、
	//		 サンプル品質、
	//		 リソースフラグ);
	const auto l_resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_format,
															 m_width,
															 m_height,
															 Constant::k_renderTextureDefaultArraySize,
															 Constant::k_renderTextureDefaultMIPLevels,
															 Constant::k_renderTextureDefaultSampleCount,
															 Constant::k_renderTextureDefaultSampleQuality,
															 D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	FWK_ASSERT_RETURN_VALUE_IF(!a_gpuMemoryAllocator.CreateTextureResource(l_resourceDesc,
																		   &l_clearValue,
																		   Constant::k_defaultDepthStencilTextureResourceState,
																		   m_gpuResource),
																		   "DepthStencilTexture用TextureResourceの作成に失敗しました。",
																		   false)

	// D3D12_DEPTH_STENCIL_VIEW_DESCについて
	// Format	     : DSVとしてみるときのフォーマット
	// ViewDimension : 2DTextureとしてDSVを作成する
	// Flags		 : 通常のDepthStencilViewとして使用する
	D3D12_DEPTH_STENCIL_VIEW_DESC l_dsvDesc = {};

	l_dsvDesc.Format		= m_format;
	l_dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	l_dsvDesc.Flags			= D3D12_DSV_FLAG_NONE;

	const auto l_dsvHandle = a_dsvDescriptorPool.FetchVALCPUOnlyCPUHandle(l_dsvStorageID);

	// CreateDepthStencilView(DSVを作りたい対象リソース、
	//						  DSV設定、
	//						  DSVを書き込むCPUディスクリプタハンドル);
	l_device->CreateDepthStencilView(m_gpuResource.m_resource.Get(), &l_dsvDesc, l_dsvHandle);

	m_currentResourceState = Constant::k_defaultDepthStencilTextureResourceState;

	m_dsvStorageID = l_dsvStorageID;

	return true;
}

nlohmann::json FWK::Graphics::DepthStencilTexture::Serialize() const
{
	return m_depthStencilTextureJsonConverter.Serialize(*this);
}

bool FWK::Graphics::DepthStencilTexture::Resize(const Device&							 a_device, 
											    const GPUMemoryAllocator&				 a_gpuMemoryAllocator, 
												const Struct::ClientSize&				 a_clientSize, 
												const UINT64&							 a_retiredFenceValue, 
													  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool, 
													  DeferredResourceReleaseQueue&		 a_deferredResourceReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!IsValidTextureSize(a_clientSize), "DepthStencilTextureのリサイズ後サイズが無効です。", false)

	if (IsSameTextureSize(a_clientSize)) { return true; }

	FWK_ASSERT_RETURN_VALUE_IF(!IsValidCurrentResourceForDeferredRelease(a_retiredFenceValue), "現在のDepthStencilTextureを遅延解放できない状態です。", false)

	DepthStencilTexture l_newDepthStencilTexture = {};

	l_newDepthStencilTexture.SetFormat		   (m_format);
	l_newDepthStencilTexture.SetWidth		   (a_clientSize.m_width);
	l_newDepthStencilTexture.SetHeight		   (a_clientSize.m_height);
	l_newDepthStencilTexture.SetIsUseWindowSize(m_isUseWindowSize);

	// 先に新しいDepthStencilTextureを作成。
	// 古いリソースを先にQueueへ移してから新規作成に失敗すると、
	// このインスタンスが有効な深度ステンシルを失ってしまうため
	FWK_ASSERT_RETURN_VALUE_IF(!l_newDepthStencilTexture.Create(a_device, a_gpuMemoryAllocator, a_dsvDescriptorPool), "リサイズ後のDepthStencilTexture作成に失敗しました。", false)

	// 新しいDepthStencilTextureの作成に成功した後で、古いリソースをDeferredReleaseへ渡す。
	FWK_ASSERT_RETURN_VALUE_IF(!PushCurrentResourceForDeferredRelease(a_retiredFenceValue, a_deferredResourceReleaseQueue), "古いDepthStencilTextureの遅延解放Queue登録に失敗しました。", false)

	*this = std::move(l_newDepthStencilTexture);

	return true;
}

bool FWK::Graphics::DepthStencilTexture::IsValidTextureSize(const Struct::ClientSize& a_clientSize) const
{
	if (a_clientSize.m_width  == Constant::k_invalidTextureWidth)  { return false; }
	if (a_clientSize.m_height == Constant::k_invalidTextureHeight) { return false; }

	return true;
}

bool FWK::Graphics::DepthStencilTexture::IsSameTextureSize(const Struct::ClientSize& a_clientSize) const
{
	if (m_width  != a_clientSize.m_width)  { return false; }
	if (m_height != a_clientSize.m_height) { return false; }

	return true;
}

bool FWK::Graphics::DepthStencilTexture::IsValidCurrentResourceForDeferredRelease(const UINT64& a_retiredFenceValue) const
{
	if (!m_gpuResource.m_resource)						     { return false; }
	if (m_dsvStorageID      == Constant::k_invalidStorageID) { return false; }
	if (a_retiredFenceValue == Constant::k_unusedFenceValue) { return false; }

	return true;
}

bool FWK::Graphics::DepthStencilTexture::PushCurrentResourceForDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(!IsValidCurrentResourceForDeferredRelease(a_retiredFenceValue), "DepthStencilTextureが無効なため、遅延解放Queueへ登録できません。", false)

	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(m_gpuResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_dsvDescriptorIndexReleaseRecord = {};

	l_dsvDescriptorIndexReleaseRecord.m_storageID		  = m_dsvStorageID;
	l_dsvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushGPUResourceRecord(std::move(l_gpuResourceReleaseRecord)),		 "DepthStencilTextureのGPUResourceを遅延解放Queueへ登録できませんでした。",		   false)
	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushDSVDescriptorIndex(std::move(l_dsvDescriptorIndexReleaseRecord)), "DepthStencilTextureのDSVDescriptorIndexを遅延解放Queueへ登録できませんでした。", false)

	// 二重解放を防ぐため、Queueへ渡したDescriptorIndexは無効化します。
	m_dsvStorageID = Constant::k_invalidStorageID;

	return true;
}