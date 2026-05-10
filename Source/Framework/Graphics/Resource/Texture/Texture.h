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

		auto GetVALStorageID() const { return m_storageID; }

	private:

		void AddTextureReference() const;

		void ReleaseTextureReference();

		TypeAlias::StorageID m_storageID;
	};
}