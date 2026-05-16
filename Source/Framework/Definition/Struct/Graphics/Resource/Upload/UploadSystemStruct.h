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

	struct BufferUploadCommand final
	{
		// バッファーへコピーするためのUpload情報
		BufferUploadRecord m_bufferUploadRecord = {};

		// Upload先のDEFAULTヒープ上BufferResource
		TypeAlias::ComPtr<ID3D12Resource2> m_destinationBuffer = nullptr;

		// Upload先のDEFAULTヒープ上BufferResource用Allocation
		TypeAlias::ComPtr<D3D12MA::Allocation> n_destinationBufferAllocation = nullptr;
	};
}