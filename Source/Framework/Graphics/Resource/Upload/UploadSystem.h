#pragma once

namespace FWK::Graphics
{
	class UploadSystem final
	{
	private:

		struct PendingUploadBuffer final
		{
			UploadBuffer m_uploadBuffer = {};
		};

	public:

		 UploadSystem() = default;
		~UploadSystem() = default;

		bool Create(const Device& a_device);

		void RecordTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer, const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList);

		UploadBuffer* CreateUploadBuffer(const UINT64 a_bufferSize, const Device& a_device);

	private:

		CopyCommandQueue     m_copyCommandQueue     = {};
		CopyCommandAllocator m_copyCommandAllocator = {};
		CopyCommandList      m_copyCommandList      = {};

		std::vector<PendingUploadBuffer> m_pendingUploadBufferList = {};
	};
}