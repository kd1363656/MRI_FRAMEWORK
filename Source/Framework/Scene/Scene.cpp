#include "Scene.h"

void FWK::Scene::PostLoadSetup()
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetMutableREFTextureSystem  ();
	
	l_textureSystem.RequestTextureLoad("Asset/Texture/Test.dds");
}