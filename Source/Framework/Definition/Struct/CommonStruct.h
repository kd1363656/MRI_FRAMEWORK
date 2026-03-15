#pragma once

namespace FWK::CommonStruct
{
	struct WindowConfig final
	{
		::Tag styleTag = CommonConstant::k_invalidStaticID;

		std::uint32_t width  = CommonConstant::k_defaultWindowWidth;
		std::uint32_t height = CommonConstant::k_defaultWindowHeight;
	};

	struct TypeInfo final
	{
		explicit TypeInfo(const TypeInfo* const a_baseInfo, const std::string_view& a_name, const std::uint32_t a_id) :
			k_baseInfo(a_baseInfo),
			k_name(a_name),
			k_id(a_id)
		{}
		~TypeInfo() = default;

		const TypeInfo* const  k_baseInfo;
		const std::string_view k_name;
		const std::uint32_t	   k_id;
	};

	// 文字列キー用の透過ハッシュ構造体
	struct StringHash final
	{
		// 透過ハッシュであることをSTLに通知するための宣言
		using is_transparent = void;

		// 受け取った文字列をstd::string_viewとして扱い、ハッシュ値を計算する
		std::size_t operator()(const std::string_view& a_key) const
		{
			return std::hash<std::string_view>{}(a_key); 
		}
	};

	struct TextureAllocation final
	{
		TextureHeapPageID textureHeapPageID = CommonConstant::k_invalidTextureHeapPageID;

		UINT64 heapOffset     = 0ULL;
		UINT64 allocationSize = 0ULL;

		Microsoft::WRL::ComPtr<ID3D12Heap> heap = nullptr;
	};

	struct TextureRecord final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource2> textureResource = nullptr;

		std::string sourcePath = std::string();

		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;

		UINT64 heapOffset     = 0ULL;
		UINT64 allocationSize = 0ULL;

		TextureID textureID = CommonConstant::k_invalidTextureID;

		TextureHeapPageID textureHeapPageID = CommonConstant::k_invalidTextureHeapPageID;

		UINT srvIndex  = CommonConstant::k_invalidDescriptorHeapIndex;
		UINT width     = 0U;
		UINT height    = 0U;
		UINT mipLevels = 0U;
		UINT arraySize = 0U;
	};
}