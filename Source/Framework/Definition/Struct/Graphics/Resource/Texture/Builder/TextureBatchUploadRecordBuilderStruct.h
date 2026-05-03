#pragma once

namespace FWK::Struct
{
	struct TextureBatchUploadRecord
	{
		Struct::TextureRecord	    m_textureRecord       = {};
		Struct::TextureUploadRecord m_textureUploadRecord = {};
		std::wstring				m_filePath			  = {};
	};
}