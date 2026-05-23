#include "SceneTexture.h"

bool FWK::Graphics::SceneTexture::Create(const Device& a_device, const GPUMemoryAllocator& a_gpuMemoryAllocator, const UINT a_width, const UINT a_height, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	m_sceneColorTexture		   = std::make_shared<RenderTargetTexture>();
	m_sceneDepthStencilTexture = std::make_shared<DepthStencilTexture>();

	// シーンのカラー用テクスチャの作成
	if (!m_sceneColorTexture->Create(a_device,
									 a_gpuMemoryAllocator,
									 k_defaultSceneColorFormat,
									 a_width,
									 a_height,
									 a_rtvDescriptorPool,
									 a_srvDescriptorPool))
	{
		assert(false && "SceneColorTextureの作成に失敗しました。");
		return false;
	}

	// シーンのデプスステンシル用テクスチャの作成
	if (!m_sceneDepthStencilTexture->Create(a_device,
											a_gpuMemoryAllocator,
											a_width,
											a_height,
											a_dsvDescriptorPool))
	{
		assert(false && "SceneDepthStencilTextureの作成に失敗しました。");
		return false;
	}

	return true;
}