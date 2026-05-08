#pragma once

namespace FWK::Struct
{
	struct TextureBatchUploadRecord final
	{
		Struct::TextureRecord	    m_textureRecord       = {};
		Struct::TextureUploadRecord m_textureUploadRecord = {};
	};
}