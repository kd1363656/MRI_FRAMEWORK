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

		static constexpr UINT k_mapSubresourceNum = 0U;
		static constexpr UINT k_unMapSubresourceNum = 0U;

		TypeAlias::ComPtr<ID3D12Resource2> m_uploadBuffer = nullptr;
	};
}