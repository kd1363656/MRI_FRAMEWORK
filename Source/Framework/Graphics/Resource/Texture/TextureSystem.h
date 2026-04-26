#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	public:

		TextureSystem () = default;
		~TextureSystem() = default;

		bool LoadTextureFile(DirectX::ScratchImage& a_scratchImage, DirectX::TexMetadata& a_texMetadata, const std::filesystem::path& a_sourcePath) const;

	private:

		TextureLoader m_textureLoader = {};
	};
}