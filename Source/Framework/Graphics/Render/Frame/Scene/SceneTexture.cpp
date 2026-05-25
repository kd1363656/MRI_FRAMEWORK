#include "SceneTexture.h"

void FWK::Graphics::SceneTexture::INIT()
{
	m_finalSceneTexture			    = std::make_shared<RenderTargetTexture>();
	m_finalSceneDepthStencilTexture = std::make_shared<DepthStencilTexture>();

	m_renderTargetTextureRecordMap.clear();
	m_depthStencilTextureRecordMap.clear();

	m_renderTargetTextureRecordList.clear();
	m_depthStencilTextureRecordList.clear();
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

	// 最終出力シーンテクスチャはウィンドウサイズに依存するべき
	m_finalSceneTexture->SetWidth (a_width);
	m_finalSceneTexture->SetHeight(a_height);

	// 最終出力シーン用テクスチャの作成
	if (!m_finalSceneTexture->Create(a_device,
									 a_gpuMemoryAllocator,
									 a_rtvDescriptorPool,
									 a_srvDescriptorPool))
	{
		assert(false && "SceneColorTextureの作成に失敗しました。");
		return false;
	}

	// 最終出力デプスステンシルテクスチャはウィンドウサイズに依存するべき
	m_finalSceneDepthStencilTexture->SetWidth (a_width);
	m_finalSceneDepthStencilTexture->SetHeight(a_height);

	// 最終出力デプスステンシル用テクスチャの作成
	if (!m_finalSceneDepthStencilTexture->Create(a_device, a_gpuMemoryAllocator, a_dsvDescriptorPool))
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

void FWK::Graphics::SceneTexture::AddRenderTargetTexture(const Struct::SceneRenderTargetTextureRecord& a_renderTargetTextureRecord)
{
	if (a_renderTargetTextureRecord.m_typeTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderTargetTexture用TypeTagが無効です。");
		return;
	}

	if (!a_renderTargetTextureRecord.m_renderTargetTexture)
	{
		assert(false && "RenderTargetTextureがnullptrです。");
		return;
	}

	if (m_renderTargetTextureRecordMap.contains(a_renderTargetTextureRecord.m_typeTag))
	{
		assert(false && "同じTypeTagのRenderTargetが既に登録されています。");
		return;
	}

	m_renderTargetTextureRecordList.emplace_back(a_renderTargetTextureRecord);
	m_renderTargetTextureRecordMap.try_emplace  (a_renderTargetTextureRecord.m_typeTag, a_renderTargetTextureRecord.m_renderTargetTexture);
}

void FWK::Graphics::SceneTexture::AddDepthStencilTexture(const Struct::SceneDepthStencilTextureRecord& a_depthStencilTextureRecord)
{
	if (a_depthStencilTextureRecord.m_typeTag == Constant::k_invalidTypeTag)
	{
		assert(false && "DepthStencilTexture用TypeTagが無効です。");
		return;
	}

	if (!a_depthStencilTextureRecord.m_depthStencilTexture)
	{
		assert(false && "DepthStencilTextureがnullptrです。");
		return;
	}

	if (m_depthStencilTextureRecordMap.contains(a_depthStencilTextureRecord.m_typeTag))
	{
		assert(false && "同じTypeTagのDepthStencilTextureが既に登録されています。");
		return;
	}

	m_depthStencilTextureRecordList.emplace_back(a_depthStencilTextureRecord);
	m_depthStencilTextureRecordMap.try_emplace  (a_depthStencilTextureRecord.m_typeTag, a_depthStencilTextureRecord.m_depthStencilTexture);
}

std::weak_ptr<FWK::Graphics::RenderTargetTexture> FWK::Graphics::SceneTexture::FindVALRenderTargetTexture(const TypeAlias::TypeTag a_typeTag) const
{
	const auto l_itr = m_renderTargetTextureRecordMap.find(a_typeTag);

	if (l_itr == m_renderTargetTextureRecordMap.end()) { return {}; }

	return l_itr->second;
}
std::weak_ptr<FWK::Graphics::DepthStencilTexture> FWK::Graphics::SceneTexture::FindVALDepthStencilTexture(const TypeAlias::TypeTag a_typeTag) const
{
	const auto l_itr = m_depthStencilTextureRecordMap.find(a_typeTag);

	if (l_itr == m_depthStencilTextureRecordMap.end()) { return {}; }

	return l_itr->second;
}