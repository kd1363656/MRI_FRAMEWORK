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

		struct UploadPage
		{
			ComPtr<ID3D12Resource2> resource   = nullptr;
			std::uint8_t*           mappedPtr  = nullptr;
			UINT64					size       = 0ULL;
			UINT64					offset     = 0ULL;    // 次に詰める位置
			UINT64					fenceValue = 0ULL;    // Flush時にセット
		};

		struct PendingUpload
		{
			std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layoutlist       = std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>();
			std::vector<UINT>								rowCountList     = std::vector<UINT>							  ();
			std::vector<UINT64>								rowSizeList      = std::vector<UINT64>							  ();
			ComPtr<ID3D12Resource2>                         textureResource  = nullptr;
			UploadPage*										uploadPage       = nullptr;
			UINT64											uploadBaseOffset = 0ULL;											   // UploadArena内の先頭オフセット
			UINT											subResourceCount = 0U;
			UINT											srvIndex         = 0U;
			CommonStruct::TextureRecord						record           = CommonStruct::TextureRecord();						// 出力用
		};

		struct UploadAllocation
		{
			UploadPage* page   = nullptr;
			UINT64      offset = 0ULL;
		};

	public:

		TextureContext () = default;
		~TextureContext() = default;

		void Init();
		
		// 拡張子から使用するローダーを自動で選択する関数
		Texture LoadFile(const std::string& a_filePath);

		// まとめてGPUへ転送
		void FlushUploads();

		UINT FetchSRVIndex(const TextureID a_id) const;

	private:

		Texture LoadTexture(const std::string& a_filePath, const TextureLoaderFunc& a_textureLoaderFunc);

		bool CreateTextureResourceAndUpload(const DirectX::TexMetadata& a_metadata, const DirectX::ScratchImage& a_image, CommonStruct::TextureRecord& a_outRecord);

		// GPUテクスチャ用ヒープ確保(CreateHeap + CreatePlacedResource)
		bool AllocateDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc,
										ComPtr<ID3D12Heap>&        a_outHeap,
										ComPtr<ID3D12Resource2>&   a_outResource);

		// 必要サイズを確保しmapped pointerとオフセットを返す
		bool AllocateUploadMemory(const UINT64& a_size, const UINT64& a_alignment, UploadAllocation& a_outAllocation);

		// GPU完了したUploadPageを開放、再利用
		void GarbageCollectUploadPage();

		bool CreateUploadPage(const UINT64 a_pageSize, UploadPage& a_outPage) const;
		
		static constexpr TextureID k_initialID        = 1U;
		static constexpr UINT      k_invalideSRVIndex = UINT_MAX;

		TextureID m_nextID = k_initialID;

		TextureLoaderMap   m_textureLoaderMap;
		TexturePathToIDMap m_pathToIDMap;
		TextureIDToSRVMap  m_textureIDToSRVIndexMap;

		std::vector<CommonStruct::TextureRecord> m_recordList;

		// バッチアップロード用
		std::vector<PendingUpload> m_pendingUploadList;
		std::vector<UploadPage>    m_uploadPageList;
	};
}