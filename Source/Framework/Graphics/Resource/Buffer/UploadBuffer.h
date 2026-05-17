#pragma once

namespace FWK::Graphics
{
	class UploadBuffer final
	{
	public:

		 UploadBuffer() = default;
		~UploadBuffer() = default;

		bool Create(const Device& a_device, const UINT64& a_bufferSize);

		std::uint8_t* Map  () const;
		void		  UnMap() const;

		D3D12_GPU_VIRTUAL_ADDRESS FetchVALGPUVirtualAddress() const;

		const auto& GetREFUploadBuffer() const { return m_uploadBuffer; }

	private:

		TypeAlias::ComPtr<ID3D12Resource2> m_uploadBuffer = nullptr;
	};
}