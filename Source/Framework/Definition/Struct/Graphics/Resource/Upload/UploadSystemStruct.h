#pragma once

namespace FWK::Struct
{
	struct TextureUploadRecord final
	{	
		// DEFAULTヒープ上のテクスチャリソースへコピーするための中間バッファ
		Graphics::UploadBuffer m_uploadBuffer = {};

		// 各サブリソースの配置情報をまとめたリスト
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> m_layoutList = {};
	};
}