#include "Texture.h"

void FWK::Graphics::Texture::Load(const std::filesystem::path& a_filePath)
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	auto& l_resourceContext = l_graphicsManager.GetREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetREFTextureSystem  ();

}