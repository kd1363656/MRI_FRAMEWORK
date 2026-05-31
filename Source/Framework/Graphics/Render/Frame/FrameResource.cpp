#include "FrameResource.h"

void FWK::Graphics::FrameResource::INIT()
{
	if (m_directCommandAllocator) { return; }

	m_directCommandAllocator = std::make_shared<DirectCommandAllocator>();

	m_renderGraphFrameResourceRegistry.INIT();
}
void FWK::Graphics::FrameResource::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_frameResourceJsonConverter.Deserialize(a_rootJson, *this);
}
void FWK::Graphics::FrameResource::PostDeserializeSetup(const Struct::WindowCONFIG& a_windowCONFIG)
{
	m_renderGraphFrameResourceRegistry.PostDeserializeSetup(a_windowCONFIG);
}
bool FWK::Graphics::FrameResource::Create(const Device&			                   a_device, 
										  const GPUMemoryAllocator&                a_gpuMemoryAllocator, 
										  const UINT				               a_width, 
										  const UINT				               a_height, 
												DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
												DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
												DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	for (const auto& [l_staticTypeID, l_constantBufferUploader] : m_constantBufferUploaderMap)
	{
		if (!l_constantBufferUploader) { continue; }

		l_constantBufferUploader->Create(a_device);
	}

	FWK_ASSERT_RETURN_VALUE_IF(!m_directCommandAllocator,					"ダイレクトコマンドアロケータが無効です。",				  false)
	FWK_ASSERT_RETURN_VALUE_IF(!m_directCommandAllocator->Create(a_device), "ダイレクトコマンドアロケータの作成処理に失敗しました。", false)

	FWK_ASSERT_RETURN_VALUE_IF(!m_renderGraphFrameResourceRegistry.Create(a_device,
																		  a_gpuMemoryAllocator,
																		  a_width,
																		  a_height,
																		  a_rtvDescriptorPool,
																		  a_srvDescriptorPool,
																		  a_dsvDescriptorPool),
																		  "FrameResource用RenderGraphResourceRegistryの作成に失敗しました。",
																		  false)

	return true;
}

void FWK::Graphics::FrameResource::BeginFrame() const
{ 
	for (const auto& [l_tag, l_constantBufferUploader] : m_constantBufferUploaderMap)
	{
		if (!l_constantBufferUploader) { continue; }

		l_constantBufferUploader->BeginFrame();
	}
}

nlohmann::json FWK::Graphics::FrameResource::Serialize() const
{
	return m_frameResourceJsonConverter.Serialize(*this);
}

bool FWK::Graphics::FrameResource::Resize(const Device&							   a_device, 
										  const GPUMemoryAllocator&				   a_gpuMemoryAllocator, 
										  const Struct::ClientSize&				   a_clientSize, 
										  const UINT64&							   a_retiredFenceValue, 
												DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
												DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
											    DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool, 
											    DeferredResourceReleaseQueue&	   a_deferredResourceReleaseQueue)
{
	// FrameResourceごとにRenderGraphResourceRegistryを持っているため、
	// ウィンドウサイズに依存するRenderTargetTexture / DepthStencilTextureも
	// FrameResourceごとResizeする必要がある
	FWK_ASSERT_RETURN_VALUE_IF(!m_renderGraphFrameResourceRegistry.Resize(a_device,		
																		  a_gpuMemoryAllocator,
																		  a_clientSize,
																		  a_retiredFenceValue,
																		  a_rtvDescriptorPool,
																		  a_srvDescriptorPool,
																		  a_dsvDescriptorPool,
																		  a_deferredResourceReleaseQueue),
																		  "FrameResource用RenderGraphResourceRegistryのリサイズに失敗しました。",
																		  false)

	return true;
}

void FWK::Graphics::FrameResource::AddConstantBufferUploader(const std::shared_ptr<ConstantBufferUploaderBase>& a_constantBufferUploader)
{
	if (!a_constantBufferUploader) { return; }

	const auto l_staticTypeID = a_constantBufferUploader->GetREFRuntimeTypeINFO().k_staticTypeID;

	m_constantBufferUploaderMap.try_emplace(l_staticTypeID, a_constantBufferUploader);
}