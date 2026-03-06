#pragma once

namespace FWK::Graphics
{
	class TextureContext final
	{
	private:

		using TextureLoaderFunc = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata&, DirectX::ScratchImage&)>;
		
		using TextureLoaderMap   = std::unordered_map<std::string, TextureLoaderFunc, CommonStruct::StringHash, std::equal_to<>>;
		using TexturePathToIDMap = std::unordered_map<std::string, TextureID,         CommonStruct::StringHash, std::equal_to<>>;
		using TextureIDToSRVMap  = std::unordered_map<TextureID,    UINT>;

	public:

		TextureContext () = default;
		~TextureContext() = default;

		void Init();
		
		// 拡張子から使用するローダーを自動で選択する関数
		Texture LoadFile(const std::string& a_filePath);

		void FlushUploads();
		
		UINT FetchSRVIndex(const TextureID a_id) const;

	private:

		static constexpr TextureID k_initialTextureID = 1U;
		static constexpr UINT      k_invalideSRVIndex = UINT_MAX;

		TextureID m_nextTextureID = k_initialTextureID;

		TextureLoaderMap   m_textureLoaderMap;
		TexturePathToIDMap m_pathToIDMap;
		TextureIDToSRVMap  m_textureIDToSRVIndexMap;

		std::vector<CommonStruct::TextureRecord> m_recordList;

		TextureUploader m_textureUploader;
	};
}