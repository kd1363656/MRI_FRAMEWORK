#pragma once

namespace FWK::Graphics
{
	class DefaultTextureBuilder final
	{
	public:

		 DefaultTextureBuilder() = default;
		~DefaultTextureBuilder() = default;

		bool CreateDefaultTextureBatchUploadRecord(const Device&				            a_device,
												   const GPUMemoryAllocator&	            a_gpuMemoryAllocator,
												   const TextureBatchUploadRecordBuilder&	a_textureBatchUploadRecordBuilder,
												   const Enum::DefaultTextureType           a_defaultTextureType,
														 DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
														 Struct::TextureBatchUploadRecord&  a_textureBatchUploadRecord) const;

	private:

		struct DefaultTextureCreateDesc final
		{
			std::array<std::uint8_t, Constant::k_defaultRGBAElementCount> m_pixel = {};

			DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;

			std::wstring_view m_name = {};
		};

		bool CreateDefaultTextureScratchImage(const DefaultTextureCreateDesc& a_defaultTextureCreateDesc, DirectX::ScratchImage& a_scratchImage) const;

		DefaultTextureCreateDesc GetVALDefaultTextureCreateDesc(const Enum::DefaultTextureType a_defaultTextureType) const;

		static constexpr std::size_t k_defaultTextureWidth     = 1ULL;
		static constexpr std::size_t k_defaultTextureHeight    = 1ULL;
		static constexpr std::size_t k_defaultTextureArraySize = 1ULL;
		static constexpr std::size_t k_defaultTextureMIPLevels = 1ULL;
	};
}