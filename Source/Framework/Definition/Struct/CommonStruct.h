#pragma once

namespace FWK::CommonStruct
{
	// 文字列キー用の透過ハッシュ構造体
	struct StringHash final
	{
		// 透過ハッシュであることをSTLに通知するための宣言
		using is_transparent = void;

		// 関数オペレーターを呼び出すことで与えられた値をハッシュ値に変換する
		std::size_t operator()(const std::string& a_key) const { return std::hash<std::string>     {}(a_key); }
		std::size_t operator()(std::string_view   a_key) const { return std::hash<std::string_view>{}(a_key); }
		std::size_t operator()(const char*        a_key) const { return std::hash<std::string_view>{}(a_key); }
	};

	struct WindowConfig final
	{
		std::uint32_t width    = CommonConstant::k_defaultWindowWidth;
		std::uint32_t height   = CommonConstant::k_defaultWindowHeight;
		::Tag		  styleTag = CommonConstant::k_invalidStaticID;
	};

	struct TextureRecord final
	{
		ComPtr<ID3D12Resource2> resource = nullptr;

		std::string filePath = std::string();

		TextureID textureID = CommonConstant::k_invalidTextureID;

		DXGI_FORMAT           format       = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;

		UINT64 haepOffset     = 0ULL;
		UINT64 allocationSize = 0ULL;

		UINT width     = 0U;
		UINT height    = 0U;
		UINT mipCount  = 0U;
		UINT srvIndex  = CommonConstant::k_invalidIndex;
		UINT pageIndex = CommonConstant::k_invalidIndex;

		bool isSharedHeap = false;
	};

	struct UploadPage final
	{
		ComPtr<ID3D12Resource2> resource = nullptr;

		std::uint8_t* mappedPtr  = nullptr;

		UINT64 size       = 0ULL;
		UINT64 offset     = 0ULL;								 // 次に詰める位置
		UINT64 fenceValue = CommonConstant::k_unusedFenceValue;
	};

	struct PendingUpload final
	{
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layoutlist   = std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>();
		std::vector<UINT>								rowCountList = std::vector<UINT>							  ();
		std::vector<UINT64>								rowSizeList  = std::vector<UINT64>							  ();

		UINT64 requiredSize     = 0ULL;
		UINT64 uploadBaseOffset = 0ULL;
		
		UINT uploadPageIndex = CommonConstant::k_invalidIndex;

		CommonStruct::TextureRecord* record = nullptr;
	};

	struct UploadAllocation final
	{
		UploadPage* page = nullptr;

		UINT64 offset = 0ULL;
	};

	struct TextureHeapPage final 
	{
		ComPtr<ID3D12Heap> heap = nullptr;

		D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

		UINT64 size     = 0ULL;
		UINT64 usedSize = 0ULL;
	};

	struct TextureAllocation final 
	{
		UINT64 offset    = 0ULL;
		UINT64 size      = 0ULL;
		UINT64 alignment = 0ULL;

		UINT pageIndex = CommonConstant::k_invalidIndex;
	};
}