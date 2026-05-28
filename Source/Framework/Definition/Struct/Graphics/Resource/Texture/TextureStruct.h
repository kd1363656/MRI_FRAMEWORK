#pragma once

namespace FWK::Graphics
{
	class TextureRecord;
}

namespace FWK::Struct
{
	struct TextureLoadResult final
	{
		std::weak_ptr<Graphics::TextureRecord> m_textureRecord = {};

		TypeAlias::StorageID m_storageID = Constant::k_invalidStorageID;
	};

	struct SpriteRECT final
	{
		std::uint32_t x      = Constant::k_defaultSpriteRECTX;
		std::uint32_t y      = Constant::k_defaultSpriteRECTY;
		std::uint32_t width  = Constant::k_defaultSpriteRECTWidth;
		std::uint32_t height = Constant::k_defaultSpriteRECTHeight;
	};
}