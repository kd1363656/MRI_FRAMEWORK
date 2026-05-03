#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	const auto& l_device = l_graphicsManager.GetREFDevice();

		  auto& l_resourceContext    = l_graphicsManager.GetMutableREFResourceContext  ();
	const auto& l_gpuMemoryAllocator = l_resourceContext.GetREFGPUMemoryAllocator      ();
	      auto& l_textureSystem      = l_resourceContext.GetMutableREFTextureSystem    ();
	      auto& l_srvDescriptorPool  = l_resourceContext.GetMutableREFSRVDescriptorPool();
	
	l_textureSystem.LoadTextureForBatchUpload(l_device, 
											  l_gpuMemoryAllocator,
											  "Asset/Texture/Test.dds",
											  l_srvDescriptorPool);
}