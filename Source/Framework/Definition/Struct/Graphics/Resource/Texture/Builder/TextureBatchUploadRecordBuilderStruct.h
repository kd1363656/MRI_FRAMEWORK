#pragma once

namespace FWK::Struct
{
	struct TextureBatchUploadRecord final
	{
		std::shared_ptr<Graphics::TextureRecord> m_textureRecord       = nullptr;
		Struct::TextureUploadRecord			     m_textureUploadRecord = {};
	};
}