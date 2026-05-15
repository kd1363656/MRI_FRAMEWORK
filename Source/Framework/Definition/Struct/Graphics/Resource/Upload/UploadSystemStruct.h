#pragma once

namespace FWK::Struct
{
	struct TextureUploadRecord final
	{	
		// 各サブリソースの配置情報をまとめたリスト
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> m_layoutList = {};

		// DEFAULTヒープ上のテクスチャリソースへコピーするための中間バッファ
		Graphics::UploadBuffer m_uploadBuffer = {};
	};

	struct BufferUploadRecord final
	{
		// DEFAULTヒープ上のBufferResourceへコピーするための中間バッファ
		Graphics::UploadBuffer m_uploadBuffer = {};

		// コピーするバッファサイズ
		UINT64 m_bufferSize = Constant::k_invalidBufferSize;
	};
}