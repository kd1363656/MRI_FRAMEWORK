#pragma once

namespace FWK::Struct
{
	struct TextureUploadRecord final
	{	
		// DEFAULTヒープ上に置くテクスチャリソース
		TypeAlias::ComPtr<ID3D12Resource2> m_textureResource = nullptr;

		// DEFAULTヒープ上のテクスチャリソースへコピーするための中間バッファ
		Graphics::UploadBuffer m_uploadBuffer = {};

		// 各サブリソースの配置情報をまとめたリスト
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> m_layoutList = {};
	};
}