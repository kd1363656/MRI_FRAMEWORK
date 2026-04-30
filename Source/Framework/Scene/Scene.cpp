#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	const auto& l_device             = l_graphicsManager.GetREFDevice         ();
	const auto& l_gpuMemoryAllocator = l_graphicsManager.GetREFResourceContext().GetREFGPUMemoryAllocator();

	auto& l_resourceContext   = l_graphicsManager.GetMutableREFResourceContext  ();
	auto& l_textureSystem     = l_resourceContext.GetMutableREFTextureSystem    ();
	auto& l_srvDescriptorHeap = l_resourceContext.GetMutableREFSRVDescriptorPool();
	auto& l_uploadSystem      = l_resourceContext.GetMutableREFUploadSystem     ();

	l_textureSystem.RegisterTexture(l_device,
									l_gpuMemoryAllocator,
									"Asset/Texture/Test.dds",
									l_srvDescriptorHeap,
									l_uploadSystem);
}