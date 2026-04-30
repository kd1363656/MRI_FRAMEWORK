#pragma once

namespace FWK::Graphics
{
	class TextureLoader final
	{
	public:

		 TextureLoader() = default;
		~TextureLoader() = default;

		bool LoadTextureFile(DirectX::ScratchImage& a_scratchImage, DirectX::TexMetadata& a_texMetadata, const std::wstring& a_filePath) const;

	private:

		static constexpr DXGI_FORMAT k_invalidTextureFormat = DXGI_FORMAT_UNKNOWN;
	};
}