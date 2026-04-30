#pragma once

namespace FWK::Struct
{
	struct TextureUploadRecord final
	{	
		TypeAlias::ComPtr<ID3D12Resource2> m_textureResource = nullptr;

		Graphics::UploadBuffer m_uploadBuffer = {};

		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> m_layoutList = {};
	};
}