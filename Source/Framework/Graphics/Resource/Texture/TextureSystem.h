#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	private:

		using TexturePathMap   = std::unordered_map<std::wstring,		  TypeAlias::TextureID, Struct::WStringHash, std::equal_to<>>;
		using TextureRecordMap = std::unordered_map<TypeAlias::TextureID, Struct::TextureRecord>;
		
	public:

		 TextureSystem() = default;
		~TextureSystem() = default;

		TypeAlias::TextureID RegisterTexture(const Device&			                  a_device,
											 const GPUMemoryAllocator&                a_gpuMemoryAllocator,
											 const std::filesystem::path&             a_filePath,
												   DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap,
												   UploadSystem&					  a_uploadSystem);

	private:
	
		static constexpr TypeAlias::TextureID k_initialNextTextureID = 0U;

		TypeAlias::TextureID GenerateTextureID();

		TextureLoader   m_textureLoader   = {};
		TextureUploader m_textureUploader = {};

		TexturePathMap   m_texturePathMap   = {};
		TextureRecordMap m_textureRecordMap = {};

		TypeAlias::TextureID m_nextTextureID = k_initialNextTextureID;
	};
}