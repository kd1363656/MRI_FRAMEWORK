#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	private:

		using TexturePathMap            = std::unordered_map<std::wstring,		   TypeAlias::TextureID, Struct::WStringHash, std::equal_to<>>;
		using TextureRecordMap          = std::unordered_map<TypeAlias::TextureID, Struct::TextureRecord>;
		using PendingTextureFilePathSet = std::unordered_set<std::wstring,		   Struct::WStringHash,  std::equal_to<>>;

	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    ();

		bool RequestTextureLoad(const std::filesystem::path& a_filePath);

		bool LoadPendingTexturesAndWait();

		nlohmann::json Serialize() const;

		auto GetVALTextureIDAllocatorCapacity() const { return m_textureIDAllocatorCapacity; }

		void SetTextureIDAllocatorCapacity(const TypeAlias::TextureID a_set) { m_textureIDAllocatorCapacity = a_set; }

	private:

		bool LoadTextureBatch();

		TextureIDAllocator		   m_textureIDAllocator			= {};
		TextureLoader			   m_textureLoader				= {};
		TextureRegistrationBuilder m_textureRegistrationBuilder = {};

		JsonConverter::TextureSystemJsonConverter m_textureSystemJsonConverter = {};

		TexturePathMap            m_texturePathMap            = {};
		TextureRecordMap          m_textureRecordMap          = {};
		PendingTextureFilePathSet m_pendingTextureFilePathSet = {};

		TypeAlias::TextureID m_textureIDAllocatorCapacity = Constant::k_defaultCreateTextureIDCapacity;
	};
}