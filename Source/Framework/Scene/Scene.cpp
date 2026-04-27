#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	auto& l_textureSystem     = l_graphicsManager.GetMutableREFResourceContext().GetMutableREFTextureSystem    ();
	auto& l_srvDescriptorHeap = l_graphicsManager.GetMutableREFResourceContext().GetMutableREFSRVDescriptorPool();

	l_textureSystem.RegisterTexture(l_srvDescriptorHeap, "Asset/Texture/Test.dds");
}