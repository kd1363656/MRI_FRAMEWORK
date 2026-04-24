#pragma once

namespace FWK::Graphics
{
	class TextureLoader final
	{
	public:

		 TextureLoader() = default;
		~TextureLoader() = default;

		bool LoadTextureFile(DirectX::ScratchImage& a_scratchImage, DirectX::TexMetadata& a_texMetadata, const std::filesystem::path& a_sourcePath) const;

	private:

		bool IsSRGBFormat (const DXGI_FORMAT a_format) const;
		bool IsUNORMFormat(const DXGI_FORMAT a_format) const;
	};
}