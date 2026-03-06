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
		ComPtr<ID3D12Resource2> resource;
		ComPtr<ID3D12Heap>      heap;

		std::string sourcePath = std::string();

		TextureID textureID = CommonConstant::k_invalidTextureID;
		
		DXGI_FORMAT			  format       = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;

		UINT srvIndex = 0U;
		UINT width    = 0U;
		UINT height   = 0U;
	};

	struct UploadPage final
	{
		ComPtr<ID3D12Resource2> resource   = nullptr;
		std::uint8_t*           mappedPtr  = nullptr;
		UINT64					size       = 0ULL;
		UINT64					offset     = 0ULL;    // 次に詰める位置
		UINT64					fenceValue = 0ULL;    // Flush時にセット
	};

	struct PendingUpload final
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

	struct UploadAllocation final
	{
		UploadPage* page   = nullptr;
		UINT64      offset = 0ULL;
	};
}