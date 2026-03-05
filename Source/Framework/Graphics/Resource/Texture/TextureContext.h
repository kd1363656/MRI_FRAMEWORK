#pragma once

namespace FWK::Graphics
{
	class TextureContext final
	{
	private:

		using TextureLoaderFunc = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata*, DirectX::ScratchImage*)>;
		
		using TextureLoaderMap   = std::unordered_map<std::string, TextureLoaderFunc, CommonStruct::StringHash, std::equal_to<>>;
		using TexturePathToIDMap = std::unordered_map<std::string, TextureID,         CommonStruct::StringHash, std::equal_to<>>;

	public:

		TextureContext () = default;
		~TextureContext() = default;

		void Init  ();
		bool Create();

		// 拡張子から使用するローダーを自動で選択する関数
		Texture LoadFile(const std::string& a_filePath);

		UINT FetchSRVIndex(const TextureID a_id) const;

	private:

		Texture LoadTexture(const std::string& a_filePath, const TextureLoaderFunc& a_textureLoaderFunc);

		bool CreateTextureResourceAndUpload(const std::string&           a_filePath,
										    const DirectX::TexMetadata&  a_metadata,
											const DirectX::ScratchImage& a_image,
											CommonStruct::TextureRecord& a_outRecord);

		// GPUテクスチャ用ヒープ確保(CreateHeap + CreatePlacedResource)
		bool AllocateDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc,
										ComPtr<ID3D12Heap>&        a_outHeap,
										ComPtr<ID3D12Resource2>    a_outResource);

		// UploadHeap
		bool CreateUploadBuffer(const UINT64 a_size, ComPtr<ID3D12Resource2>& a_outUpload);

		void CreateSRV(const CommonStruct::TextureRecord& a_record);

		static constexpr TextureID k_initialID = 1U;

		TextureID m_nextID = k_initialID;

		TextureLoaderMap   m_textureLoaderMap;
		TexturePathToIDMap m_pathToIDMap;

		std::vector<CommonStruct::TextureRecord> m_recordList;
	};
}