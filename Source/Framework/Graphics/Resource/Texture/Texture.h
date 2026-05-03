#pragma once

namespace FWK::Graphics
{
	class Texture final
	{
	public:
		
		 Texture() = default;
		~Texture() = default;

		void Load(const std::filesystem::path& a_filePath);

		auto GetVALTextureID() const { return m_textureID; }

	private:

		TypeAlias::TextureID m_textureID = Constant::k_invalidTextureID;
	};
}