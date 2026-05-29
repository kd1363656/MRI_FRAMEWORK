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

	if (const auto& l_resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_format,
																  m_width,
																  m_height,
																  Constant::k_renderTextureDefaultArraySize,
																  Constant::k_renderTextureDefaultMIPLevels,
																  Constant::k_renderTextureDefaultSampleCount,
																  Constant::k_renderTextureDefaultSampleQuality,
																  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		!a_gpuMemoryAllocator.CreateTextureResource(l_resourceDesc,
													&l_clearValue,
													D3D12_RESOURCE_STATE_RENDER_TARGET,
													m_gpuResource))
	{
		assert(false && "RenderTargetTexture用TextureResourceの作成に失敗しました。");
		return false;
	}

	m_currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	if (!CreateRenderTargetView(a_device, a_rtvDescriptorPool))
	{
		assert(false && "RenderTargetTexture用RTVの作成に失敗しました。");
		return false;
	}

	if (!CreateShaderResourceView(a_device, a_srvDescriptorPool))
	{
		assert(false && "RenderTargetTexture用SRVの作成に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::RenderTargetTexture::Serialize() const
{
	return m_renderTargetTextureJsonConverter.Serialize(*this);
}

bool FWK::Graphics::RenderTargetTexture::CreateRenderTargetView(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、RenderTargetTexture用のRTVの作成に失敗しました。");
		return false;
	}

	const auto l_rtvStorageID = a_rtvDescriptorPool.Allocate();

	if (l_rtvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "RTVStorageIDの確保に失敗しました。");
		return false;
	}

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

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、RenderTargetTexture用のSRVの作成に失敗しました。");
		return false;
	}

	const auto l_srvStorageID = a_srvDescriptorPool.Allocate();

	if (l_srvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "SRVStorageIDの確保に失敗しました。");
		return false;
	}

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
	l_srvDesc.Texture2D.MostDetailedMip     = Constant::k_textureSRVMostDetailedMIP;
	l_srvDesc.Texture2D.MipLevels           = Constant::k_renderTextureDefaultMIPLevels;
	l_srvDesc.Texture2D.PlaneSlice		    = Constant::k_textureSRVPlaneSlice;
	l_srvDesc.Texture2D.ResourceMinLODClamp = Constant::k_textureSRVResourceMINLODClamp;

	const auto l_cpuOnlyCPUHandle = a_srvDescriptorPool.FetchVALCPUOnlyCPUHandle(l_srvStorageID);

	// CreateShaderResourceView(SRVを作りたい対象リソース、
	//							SRV設定、
	//							SRVを書き込むCPUディスクリプタハンドル);

	l_device->CreateShaderResourceView(m_gpuResource.m_resource.Get(), &l_srvDesc, l_cpuOnlyCPUHandle);

	if (!a_srvDescriptorPool.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(l_srvStorageID, a_device))
	{
		a_srvDescriptorPool.Release(l_srvStorageID);

		assert(false && "CPUOnlyからShaderVisibleSRVへのコピーに失敗したため、RenderTargetTexture用SRVの作成に失敗しました。");
		return false;
	}

	m_srvStorageID = l_srvStorageID;

	return true;
}