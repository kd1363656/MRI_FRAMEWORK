#pragma once

namespace FWK::Graphics
{
	class UploadBuffer final
	{
	public:

		 UploadBuffer() = default;
		~UploadBuffer() = default;

		bool Create(const Device& a_device, const UINT64& a_bufferSize);

		void BeginFrame();

		std::size_t AllocateCurrentBufferIndex();

		std::uint8_t* Map  () const;
		void		  UnMap() const;

		D3D12_GPU_VIRTUAL_ADDRESS FetchVALGPUVirtualAddress() const;

		const auto& GetREFUploadBuffer() const { return m_uploadBuffer; }

	private:

		static constexpr D3D12_GPU_VIRTUAL_ADDRESS k_invalidGPUVirtualAddress = 0ULL;

		static constexpr size_t k_initialBufferIndex = 0ULL;

		TypeAlias::ComPtr<ID3D12Resource2> m_uploadBuffer = nullptr;

		std::size_t m_currentBufferIndex = k_initialBufferIndex;
	};
}