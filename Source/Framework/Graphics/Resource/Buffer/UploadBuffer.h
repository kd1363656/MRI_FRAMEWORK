#pragma once

namespace FWK::Graphics
{
	class UploadBuffer final
	{
	public:

		 UploadBuffer() = default;
		~UploadBuffer() = default;

		bool Create(const UINT64& a_bufferSize, const Device& a_device);

		std::uint8_t* Map  () const;
		void		  UnMap() const;

		const auto& GetREFUploadBuffer() const { return m_uploadBuffer; }

	private:

		static constexpr D3D12_HEAP_TYPE       k_uploadBufferHeapType             = D3D12_HEAP_TYPE_UPLOAD;
		static constexpr D3D12_HEAP_FLAGS      k_defaultUploadBufferHeapFlags     = D3D12_HEAP_FLAG_NONE;
		static constexpr D3D12_RESOURCE_STATES k_uploadBufferInitialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

		static constexpr UINT64 k_invalidBufferSize = 0ULL;

		static constexpr UINT k_bufferSubresourceIndex = 0U;
		
		TypeAlias::ComPtr<ID3D12Resource2> m_uploadBuffer = nullptr;
	};
}