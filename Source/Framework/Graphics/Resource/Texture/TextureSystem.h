#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	private:

		using TexturePathMap            = std::unordered_map<std::wstring,		   TypeAlias::TextureID, Struct::WStringHash, std::equal_to<>>;
		using TextureRecordMap          = std::unordered_map<TypeAlias::TextureID, Struct::TextureRecord>;
		using PendingTextureFilePathSet = std::unordered_set<std::wstring,		   Struct::WStringHash, std::equal_to<>>;

		struct PendingTextureRegisterRecord final
		{
			UINT m_srvIndex = Constant::k_invalidDescriptorHeapIndex;

			Struct::TextureRecord		m_textureRecord		  = {};
			Struct::TextureUploadRecord m_textureUploadRecord = {};

			TypeAlias::TextureID m_textureID = Constant::k_invalidTextureID;

			std::wstring m_filePath = {};
		};

	public:

		 TextureSystem() = default;
		~TextureSystem() = default;
		
		bool RequestTextureLoad(const std::filesystem::path& a_filePath);

		bool ProcessPendingTextureLoadsAndWait(const Device&		                    a_device,
										       const GPUMemoryAllocator&                a_gpuMemoryAllocator,
												     DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
												     UploadSystem&                      a_uploadSystem);

		bool RegisterTextureBatch(const Device&				                a_device,
								  const GPUMemoryAllocator&                 a_gpuMemoryAllocator,
								  const PendingTextureFilePathSet&			a_filePathSet,
										DescriptorPool<SRVDescriptorHeap>&  a_srvDescriptorPool,
										UploadSystem&						a_uploadSystem);

	private:
	
		static constexpr TypeAlias::TextureID k_initialNextTextureID = 0U;

		TypeAlias::TextureID GenerateTextureID();

		TextureLoader			   m_textureLoader				= {};
		TextureUploadRecordBuilder m_textureUploadRecordBuilder = {};
		
		TexturePathMap   m_texturePathMap   = {};
		TextureRecordMap m_textureRecordMap = {};

		PendingTextureFilePathSet m_pendingTextureFilePathSet = {};

		TypeAlias::TextureID m_nextTextureID = k_initialNextTextureID;
	};
}