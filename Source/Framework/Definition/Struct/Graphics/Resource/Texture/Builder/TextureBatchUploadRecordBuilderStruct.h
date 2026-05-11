#pragma once

namespace FWK::Struct
{
	struct TextureBatchUploadRecord final
	{
		std::shared_ptr<Struct::TextureRecord> m_textureRecord       = nullptr;
		Struct::TextureUploadRecord			   m_textureUploadRecord = {};
	};
}