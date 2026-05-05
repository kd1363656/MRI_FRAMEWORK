#pragma once

namespace FWK::Graphics
{
	class Texture final
	{
	public:
		
		 Texture();
		 Texture(const Texture&  a_other);
		 Texture(	   Texture&& a_other) noexcept;
		~Texture();

		Texture& operator=(const Texture&  a_other);
		Texture& operator=(		 Texture&& a_other) noexcept;

		void Load(const std::filesystem::path& a_filePath);

		auto GetVALTextureID() const { return m_textureID; }

	private:

		void AddTextureReference() const;

		void ReleaseTextureReference();

		TypeAlias::TextureID m_textureID;
	};
}