#pragma once

namespace FWK::Struct
{
	struct TextureRecord final : public AssetRecordBase
	{
		TypeAlias::ComPtr<ID3D12Resource2>     m_textureResource = nullptr;
		TypeAlias::ComPtr<D3D12MA::Allocation> m_allocation      = nullptr;

		D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;

		TypeAlias::StorageID m_srvStorageID = Constant::k_invalidStorageID;
	};

	struct SpriteRECT final
	{
		std::uint32_t x      = Constant::k_defaultSpriteRECTX;
		std::uint32_t y      = Constant::k_defaultSpriteRECTY;
		std::uint32_t width  = Constant::k_defaultSpriteRECTWidth;
		std::uint32_t height = Constant::k_defaultSpriteRECTHeight;
	};
}