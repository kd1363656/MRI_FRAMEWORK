#include "SceneTexture.h"

void FWK::Graphics::SceneTexture::INIT()
{
	m_finalSceneTexture			    = std::make_shared<RenderTargetTexture>();
	m_finalSceneDepthStencilTexture = std::make_shared<DepthStencilTexture>();
}

void FWK::Graphics::SceneTexture::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_sceneTextureJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::SceneTexture::Create(const Device&							  a_device, 
									     const GPUMemoryAllocator&				  a_gpuMemoryAllocator, 
										 const UINT								  a_width, 
										 const UINT								  a_height, 
											   DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool, 
											   DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
											   DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	
	// シーンのカラー用テクスチャの作成
	if (!m_finalSceneTexture->Create(a_device,
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
	if (!m_finalSceneDepthStencilTexture->Create(a_device,
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

nlohmann::json FWK::Graphics::SceneTexture::Serialize() const
{
	return m_sceneTextureJsonConverter.Serialize(*this);
}