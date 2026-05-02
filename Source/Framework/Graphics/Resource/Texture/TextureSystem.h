#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	private:

		using TexturePathMap            = std::unordered_map<std::wstring,		   TypeAlias::TextureID, Struct::WStringHash, std::equal_to<>>;
		using TextureRecordMap          = std::unordered_map<TypeAlias::TextureID, Struct::TextureRecord>;
		using PendingTextureFilePathSet = std::unordered_set<std::wstring,		   Struct::WStringHash, std::equal_to<>>;

	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		bool RequestTextureLoad(const std::filesystem::path& a_filePath);

	private:
	
		static constexpr TypeAlias::TextureID k_initialNextTextureID = 0U;

		TypeAlias::TextureID GenerateTextureID();

		TextureLoader			   m_textureLoader				= {};
		TextureRegistrationBuilder m_textureRegistrationBuilder = {};
		
		TexturePathMap   m_texturePathMap   = {};
		TextureRecordMap m_textureRecordMap = {};

		PendingTextureFilePathSet m_pendingTextureFilePathSet = {};

		TypeAlias::TextureID m_nextTextureID = k_initialNextTextureID;
	};
}