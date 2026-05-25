#include "DepthStencilTexture.h"

void FWK::Graphics::DepthStencilTexture::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_depthStencilTextureJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::DepthStencilTexture::Create(const Graphics::Device& a_device, const GPUMemoryAllocator& a_gpuMemoryAllocator, DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、DepthStencilTextureの作成に失敗しました。");
		return false;
	}

	const auto l_dsvStorageID = a_dsvDescriptorPool.Allocate();

	if (l_dsvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "DSVStorageIDの確認に失敗しました。");
		return false;
	}

	// D3D12_CLEAR_VALUEについて
	// Format			    : クリア対象リソースのフォーマット
	// DepthStencil.Depth   : 深度バッファをクリアする値
	// DepthStencil.Stencil : ステンシルバッファをクリアする値
	D3D12_CLEAR_VALUE l_clearValue = {};
	
	l_clearValue.Format				  = m_format;
	l_clearValue.DepthStencil.Depth   = Constant::k_defaultDepthClearValue;
	l_clearValue.DepthStencil.Stencil = Constant::k_defaultStencilClearValue;

	// D3D12_RESOURCE_DESCについて
	// Tex2D(フォーマット、
	//		 幅、
	//		 高さ、
	//		 配列数、
	//		 Mip数、
	//		 サンプル数、
	//		 サンプル品質、
	//		 リソースフラグ);

	// 書き込み用深度テクスチャの作成
	if (const auto l_resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_format,
															     m_width,
															     m_height,
															     Constant::k_renderTextureDefaultArraySize,
															     Constant::k_renderTextureDefaultMIPLevels,
															     Constant::k_renderTextureDefaultSampleCount,
															     Constant::k_renderTextureDefaultSampleQuality,
															     D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		!a_gpuMemoryAllocator.CreateTextureResource(l_resourceDesc,
													&l_clearValue,
													Constant::k_defaultDepthStencilTextureResourceState,
												    m_gpuResource))
	{
		a_dsvDescriptorPool.Release(l_dsvStorageID);

		assert(false && "DepthStencilTexture用TextureResourceの作成に失敗しました。");
		return false;
	}

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