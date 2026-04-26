#include "TextureSystem.h"

bool FWK::Graphics::TextureSystem::LoadTextureFile(DirectX::ScratchImage& a_scratchImage, DirectX::TexMetadata& a_texMetadata, const std::filesystem::path& a_sourcePath) const
{
	return m_textureLoader.LoadTextureFile(a_scratchImage, a_texMetadata, a_sourcePath);
}