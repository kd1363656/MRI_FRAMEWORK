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
	FWK_ASSERT_RETURN_VALUE_IF(!m_finalSceneTexture,			 "最終シーン出力テクスチャがインスタンス化されていませんでした。",			 false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_finalSceneDepthStencilTexture, "最終デプスステンシル出力テクスチャがインスタンス化されていませんでした。", false)

	// 最終出力シーンテクスチャはウィンドウサイズに依存するべき
	m_finalSceneTexture->SetWidth (a_width);
	m_finalSceneTexture->SetHeight(a_height);

	// 最終出力シーン用テクスチャの作成
	FWK_ASSERT_RETURN_VALUE_IF(!m_finalSceneTexture->Create(a_device,
															a_gpuMemoryAllocator,
															a_rtvDescriptorPool,
															a_srvDescriptorPool),
															"SceneColorTextureの作成に失敗しました。",
														    false)

	// 最終出力デプスステンシルテクスチャはウィンドウサイズに依存するべき
	m_finalSceneDepthStencilTexture->SetWidth (a_width);
	m_finalSceneDepthStencilTexture->SetHeight(a_height);

	// 最終出力デプスステンシル用テクスチャの作成
	FWK_ASSERT_RETURN_VALUE_IF(!m_finalSceneDepthStencilTexture->Create(a_device, a_gpuMemoryAllocator, a_dsvDescriptorPool), "SceneDepthStencilTextureの作成に失敗しました。", false)

	// マルチパスレンダリング用レンダーターゲットテクスチャの作成
	for (const auto& l_renderTargetTextureRecord : m_renderTargetTextureRecordList)
	{
		auto& l_renderTargetTexture = l_renderTargetTextureRecord.m_renderTargetTexture;

		FWK_ASSERT_RETURN_VALUE_IF(!l_renderTargetTexture, "SceneTexture管理RenderTargetTextureがnullptrです。", false)

		// 幅が0ならウィンドウサイズを安全のためにセットする
		if (l_renderTargetTexture->GetWidth() == Constant::k_invalidRenderTextureWidth)
		{
			l_renderTargetTexture->SetWidth(a_width);
		}

		// 高さが0ならウィンドウサイズ安全のためにセットする
		if (l_renderTargetTexture->GetHeight() == Constant::k_invalidRenderTextureHeight)
		{
			l_renderTargetTexture->SetHeight(a_height);
		}

		FWK_ASSERT_RETURN_VALUE_IF(!l_renderTargetTexture->Create(a_device,
																  a_gpuMemoryAllocator,
																  a_rtvDescriptorPool,
																  a_srvDescriptorPool),
																  "SceneTexture管理RenderTargetTextureの作成に失敗しました。",
																  false)

	}

	// マルチパスレンダリング用デプスステンシルテクスチャの作成
	for (const auto& l_depthStencilTextureRecord : m_depthStencilTextureRecordList)
	{
		auto& l_depthStencilTexture = l_depthStencilTextureRecord.m_depthStencilTexture;

		FWK_ASSERT_RETURN_VALUE_IF(!l_depthStencilTexture, "SceneTexture管理DepthStencilTextureがnullptrです。", false)

		// 幅が0ならウィンドウサイズを安全のためにセットする
		if (l_depthStencilTexture->GetWidth() == Constant::k_invalidDepthStencilTextureWidth)
		{
			l_depthStencilTexture->SetWidth(a_width);
		}

		// 高さが0ならウィンドウサイズ安全のためにセットする
		if (l_depthStencilTexture->GetHeight() == Constant::k_invalidDepthStencilTextureHeight)
		{
			l_depthStencilTexture->SetHeight(a_height);
		}

		FWK_ASSERT_RETURN_VALUE_IF(!l_depthStencilTexture->Create(a_device, a_gpuMemoryAllocator, a_dsvDescriptorPool), "SceneTexture管理DepthStencilTextureの作成に失敗しました。", false)

	}

	return true;
}

nlohmann::json FWK::Graphics::SceneTexture::Serialize() const
{
	return m_sceneTextureJsonConverter.Serialize(*this);
}

void FWK::Graphics::SceneTexture::AddRenderTargetTexture(const Struct::SceneRenderTargetTextureRecord& a_renderTargetTextureRecord)
{
	FWK_ASSERT_RETURN_IF(a_renderTargetTextureRecord.m_typeTag == Constant::k_invalidTypeTag,			 "RenderTargetTexture用TypeTagが無効です。")
	FWK_ASSERT_RETURN_IF(!a_renderTargetTextureRecord.m_renderTargetTexture,							 "RenderTargetTextureがnullptrです。")
	FWK_ASSERT_RETURN_IF(m_renderTargetTextureRecordMap.contains(a_renderTargetTextureRecord.m_typeTag), "同じTypeTagのRenderTargetが既に登録されています。")

	m_renderTargetTextureRecordList.emplace_back(a_renderTargetTextureRecord);
	m_renderTargetTextureRecordMap.try_emplace  (a_renderTargetTextureRecord.m_typeTag, a_renderTargetTextureRecord.m_renderTargetTexture);
}

void FWK::Graphics::SceneTexture::AddDepthStencilTexture(const Struct::SceneDepthStencilTextureRecord& a_depthStencilTextureRecord)
{
	FWK_ASSERT_RETURN_IF(a_depthStencilTextureRecord.m_typeTag == Constant::k_invalidTypeTag,			 "DepthStencilTexture用TypeTagが無効です。")
	FWK_ASSERT_RETURN_IF(!a_depthStencilTextureRecord.m_depthStencilTexture,							 "DepthStencilTextureがnullptrです。")
	FWK_ASSERT_RETURN_IF(m_depthStencilTextureRecordMap.contains(a_depthStencilTextureRecord.m_typeTag), "同じTypeTagのDepthStencilTextureが既に登録されています。")

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