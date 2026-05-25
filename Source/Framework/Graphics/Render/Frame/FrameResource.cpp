#include "FrameResource.h"

void FWK::Graphics::FrameResource::INIT()
{
	if (m_directCommandAllocator) { return; }

	m_directCommandAllocator = std::make_shared<DirectCommandAllocator>();

	m_renderGraphResourceRegistry.INIT();
}
void FWK::Graphics::FrameResource::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_frameResourceJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::FrameResource::Create(const Device&			                   a_device, 
										  const GPUMemoryAllocator&                a_gpuMemoryAllocator, 
										  const UINT				               a_width, 
										  const UINT				               a_height, 
												DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
												DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
												DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool)
{
	for (const auto& [l_staticTypeID, l_constantBuffer] : m_constantBufferMap)
	{
		if (!l_constantBuffer) { continue; }

		l_constantBuffer->Create(a_device);
	}

	if (!m_directCommandAllocator)
	{
		assert(false && "ダイレクトコマンドアロケータが無効です。");
		return false; 
	}

	if (!m_directCommandAllocator->Create(a_device))
	{
		assert(false && "ダイレクトコマンドアロケータの作成処理に失敗しました。");
		return false;
	}

	if (!m_renderGraphResourceRegistry.Create(a_device,
											  a_gpuMemoryAllocator,
											  a_width,
											  a_height,
											  a_rtvDescriptorPool,
											  a_srvDescriptorPool,
											  a_dsvDescriptorPool))
	{
		assert(false && "FrameResource用RenderGraphResourceRegistryの作成に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::FrameResource::Serialize() const
{
	return m_frameResourceJsonConverter.Serialize(*this);
}

void FWK::Graphics::FrameResource::AddConstantBuffer(const std::shared_ptr<ConstantBufferBase>& a_constantBuffer)
{
	if (!a_constantBuffer) { return; }

	const auto l_staticTypeID = a_constantBuffer->GetRuntimeTypeINFO().k_staticTypeID;

	m_constantBufferMap.try_emplace(l_staticTypeID, a_constantBuffer);
}