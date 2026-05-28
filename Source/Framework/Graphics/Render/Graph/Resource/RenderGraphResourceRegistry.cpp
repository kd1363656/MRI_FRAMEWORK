#include "RenderGraphResourceRegistry.h"

void FWK::Graphics::RenderGraphResourceRegistry::INIT()
{
	m_renderTargetTextureResourceRecordList.clear();
	m_renderTargetTextureResourceRecordMap.clear ();
	
	m_depthStencilTextureResourceRecordList.clear();
	m_depthStencilTextureResourceRecordMap.clear ();
}
void FWK::Graphics::RenderGraphResourceRegistry::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_renderGraphResourceRegistryJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::RenderGraphResourceRegistry::Create(const Device&							 a_device, 
													    const GPUMemoryAllocator&				 a_gpuMemoryAllocator, 
														const UINT								 a_width,
														const UINT								 a_height,
															  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorHeap, 
															  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap, 
															  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	for (const auto& l_renderTargetTextureResourceRecord : m_renderTargetTextureResourceRecordList)
	{
		if (!l_renderTargetTextureResourceRecord)
		{
			assert(false && "RenderGraph管理RenderTargetTextureResourceRecordがnullptrです。");
			return false;
		}

		if (!CreateRenderTargetTexture(a_device,
									   a_gpuMemoryAllocator,
								       a_width,
								   	   a_height,
									   *l_renderTargetTextureResourceRecord,
									   a_rtvDescriptorHeap,
									   a_srvDescriptorHeap))
		{
			assert(false && "RenderGraph管理RenderTargetTextureの作成に失敗しました。");
			return false;
		}
	}

	for (const auto& l_depthStencilTextureResourceRecord : m_depthStencilTextureResourceRecordList)
	{
		if (!l_depthStencilTextureResourceRecord)
		{
			assert(false && "RenderGraph管理DepthStencilTextureResourceRecordがnullptrです。");
			return false;
		}

		if (!CreateDepthStencilTexture(a_device,
									   a_gpuMemoryAllocator,
									   a_width,
									   a_height,
									   *l_depthStencilTextureResourceRecord,
									   a_dsvDescriptorPool))
		{
			assert(false && "RenderGraph管理DepthStencilTextureの作成に失敗しました。");
			return false;
		}
	}

	return true;
}

nlohmann::json FWK::Graphics::RenderGraphResourceRegistry::Serialize() const
{
	return m_renderGraphResourceRegistryJsonConverter.Serialize(*this);
}

void FWK::Graphics::RenderGraphResourceRegistry::AddRenderTargetTexture(const std::shared_ptr<Struct::RenderGraphRenderTargetTextureResourceRecord>& a_renderTargetTextureResourceRecord)
{
	if (!a_renderTargetTextureResourceRecord)
	{
		assert(false && "RenderGraph管理RenderTargetTextureResourceRecordがnullptrです。");
		return;
	}

	if (a_renderTargetTextureResourceRecord->m_textureTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderGraph管理RenderTargetTextureResourceRecordのTextureTagが無効です。");
		return;
	}

	if (!a_renderTargetTextureResourceRecord->m_renderTargetTexture)
	{
		assert(false && "RenderGraph管理RenderTargetTextureがnullptrです。");
		return;
	}

	if (m_renderTargetTextureResourceRecordMap.contains(a_renderTargetTextureResourceRecord->m_textureTag)) { return; }

	m_renderTargetTextureResourceRecordList.emplace_back(a_renderTargetTextureResourceRecord);
	m_renderTargetTextureResourceRecordMap.try_emplace  (a_renderTargetTextureResourceRecord->m_textureTag, a_renderTargetTextureResourceRecord);

}
void FWK::Graphics::RenderGraphResourceRegistry::AddDepthStencilTexture(const std::shared_ptr<Struct::RenderGraphDepthStencilTextureResourceRecord>& a_depthStencilTextureResourceRecord)
{
	if (!a_depthStencilTextureResourceRecord)
	{
		assert(false && "RenderGraph管理DepthStencilTextureResourceRecordがnullptrです。");
		return;
	}

	if (a_depthStencilTextureResourceRecord->m_textureTag == Constant::k_invalidTypeTag)
	{
		assert(false && "RenderGraph管理DepthStencilTextureResourceRecordのTextureTagが無効です。");
		return;
	}

	if (!a_depthStencilTextureResourceRecord->m_depthStencilTexture)
	{
		assert(false && "RenderGraph管理DepthStencilTextureがnullptrです。");
		return;
	}

	if (m_depthStencilTextureResourceRecordMap.contains(a_depthStencilTextureResourceRecord->m_textureTag)) { return; }

	m_depthStencilTextureResourceRecordList.emplace_back(a_depthStencilTextureResourceRecord);
	m_depthStencilTextureResourceRecordMap.try_emplace  (a_depthStencilTextureResourceRecord->m_textureTag, a_depthStencilTextureResourceRecord);
}

std::weak_ptr<FWK::Struct::RenderGraphRenderTargetTextureResourceRecord> FWK::Graphics::RenderGraphResourceRegistry::FindVALRenderTargetTexture(const TypeAlias::TypeTag a_textureTag) const
{
	const auto& l_itr = m_renderTargetTextureResourceRecordMap.find(a_textureTag);

	if (l_itr == m_renderTargetTextureResourceRecordMap.end()) { return {}; }

	return l_itr->second;
}
std::weak_ptr<FWK::Struct::RenderGraphDepthStencilTextureResourceRecord> FWK::Graphics::RenderGraphResourceRegistry::FindVALDepthStencilTexture(const TypeAlias::TypeTag a_textureTag) const
{
	const auto& l_itr = m_depthStencilTextureResourceRecordMap.find(a_textureTag);

	if (l_itr == m_depthStencilTextureResourceRecordMap.end()) { return {}; }

	return l_itr->second;
}

bool FWK::Graphics::RenderGraphResourceRegistry::CreateRenderTargetTexture(const Device&											   a_device, 
																		   const GPUMemoryAllocator&								   a_gpuMemoryAllocator, 
																		   const UINT												   a_width, 
																		   const UINT												   a_height, 
																				 Struct::RenderGraphRenderTargetTextureResourceRecord& a_renderTargetTextureResourceRecord, 
																				 DescriptorPool<RTVDescriptorHeap>&					   a_rtvDescriptorPool, 
																				 DescriptorPool<SRVDescriptorHeap>&					   a_srvDescriptorPool)
{
	const auto& l_renderTargetTexture = a_renderTargetTextureResourceRecord.m_renderTargetTexture;

	if (!l_renderTargetTexture)
	{
		assert(false && "RenderGraph管理RenderTargetTextureがnullptrです。");
		return false;
	}

	// 幅が0ならウィンドウサイズを安全のためにセットする
	if (l_renderTargetTexture->GetWidth() == Constant::k_defaultRenderTextureWidth)
	{
		l_renderTargetTexture->SetWidth(a_width);
	}

	// 高さが0ならウィンドウサイズを安全のためにセットする
	if (l_renderTargetTexture->GetHeight() == Constant::k_defaultRenderTextureHeight)
	{
		l_renderTargetTexture->SetHeight(a_height);
	}

	if (!l_renderTargetTexture->Create(a_device,
									   a_gpuMemoryAllocator,	
									   a_rtvDescriptorPool,
									   a_srvDescriptorPool))
	{
		assert(false && "RenderGraph管理RenderTargetTextureの作成に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Graphics::RenderGraphResourceRegistry::CreateDepthStencilTexture(const Device&											   a_device, 
																		   const GPUMemoryAllocator&								   a_gpuMemoryAllocator, 
																		   const UINT												   a_width, 
																		   const UINT												   a_height, 
																				 Struct::RenderGraphDepthStencilTextureResourceRecord& a_depthStencilTextureResourceRecord, 
																				 DescriptorPool<DSVDescriptorHeap>&					   a_dsvDescriptorPool)
{
	auto& l_depthStencilTexture = a_depthStencilTextureResourceRecord.m_depthStencilTexture;

	if (!l_depthStencilTexture)
	{
		assert(false && "RenderGraph管理DepthStencilTextureがnullptrです。");
		return false;
	}

	// 幅が0ならウィンドウサイズを安全のためにセットする
	if (l_depthStencilTexture->GetWidth() == Constant::k_defaultDepthStencilTextureWidth)
	{
		l_depthStencilTexture->SetWidth(a_width);
	}

	// 高さが0ならウィンドウサイズを安全のためにセットする
	if (l_depthStencilTexture->GetHeight() == Constant::k_defaultDepthStencilTextureHeight)
	{
		l_depthStencilTexture->SetHeight(a_height);
	}

	if (!l_depthStencilTexture->Create(a_device, a_gpuMemoryAllocator, a_dsvDescriptorPool))
	{
		assert(false && "RenderGraph管理DepthStencilTextureの作成に失敗しました。");
		return false;
	}

	return true;
}