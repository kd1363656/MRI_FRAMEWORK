#include "SceneTexture.h"

void FWK::Graphics::SceneTexture::INIT()
{
	m_finalSceneTexture			    = std::make_shared<RenderTargetTexture>();
	m_finalSceneDepthStencilTexture = std::make_shared<DepthStencilTexture>();

	m_renderTargetTextureMap.clear();
	m_depthStencilTextureMap.clear();

	m_renderTargetTextureList.clear();
	m_depthStencilTextureList.clear();
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
	if (!m_finalSceneTexture) 
	{
		assert(false && "最終シーン出力テクスチャがインスタンス化されていませんでした。");
		return false; 
	}

	if (!m_finalSceneDepthStencilTexture)
	{
		assert(false && "最終デプスステンシル出力テクスチャがインスタンス化されていませんでした。");
		return false;
	}

	// 最終出力シーン用テクスチャの作成
	if (!m_finalSceneTexture->Create(a_device,
									 a_gpuMemoryAllocator,
									 a_rtvDescriptorPool,
									 a_srvDescriptorPool))
	{
		assert(false && "SceneColorTextureの作成に失敗しました。");
		return false;
	}

	// 最終出力デプスステンシル用テクスチャの作成
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

void FWK::Graphics::SceneTexture::AddRenderTargetTexture(const TypeAlias::TypeTag a_typeTag, const std::shared_ptr<RenderTargetTexture>& a_renderTargetTexture)
{
	if (a_typeTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderTargetTexture用TypeTagが無効です。");
		return;
	}

	if (!a_renderTargetTexture)
	{
		assert(false && "RenderTargetTextureがnullptrです。");
		return;
	}

	if (m_renderTargetTextureMap.contains(a_typeTag))
	{
		assert(false && "同じTypeTagのRenderTargetが既に登録されています。");
		return;
	}

	m_renderTargetTextureList.emplace_back(a_renderTargetTexture);
	m_renderTargetTextureMap.try_emplace  (a_typeTag, a_renderTargetTexture);
}
void FWK::Graphics::SceneTexture::AddDepthStencilTexture(const TypeAlias::TypeTag a_typeTag, const std::shared_ptr<DepthStencilTexture>& a_depthStencilTexture)
{
	if (a_typeTag == Constant::k_invalidTypeTag)
	{
		assert(false && "DepthStencilTexture用TypeTagが無効です。");
		return;
	}

	if (a_depthStencilTexture)
	{
		assert(false && "DepthStencilTextureがnullptrです。");
		return;
	}

	if (m_renderTargetTextureMap.contains(a_typeTag))
	{
		assert(false && "同じTypeTagのDepthStencilTextureが既に登録されています。");
		return;
	}

	m_depthStencilTextureList.emplace_back(a_depthStencilTexture);
	m_depthStencilTextureMap.try_emplace  (a_typeTag, a_depthStencilTexture);
}

std::weak_ptr<FWK::Graphics::RenderTargetTexture> FWK::Graphics::SceneTexture::FindVALRenderTargetTexture(const TypeAlias::TypeTag a_typeTag) const
{
	const auto l_itr = m_renderTargetTextureMap.find(a_typeTag);

	if (l_itr == m_renderTargetTextureMap.end()) { return {}; }

	return l_itr->second;
}
std::weak_ptr<FWK::Graphics::DepthStencilTexture> FWK::Graphics::SceneTexture::FindVALDepthStencilTexture(const TypeAlias::TypeTag a_typeTag) const
{
	const auto l_itr = m_depthStencilTextureMap.find(a_typeTag);

	if (l_itr == m_depthStencilTextureMap.end()) { return {}; }

	return l_itr->second;
}