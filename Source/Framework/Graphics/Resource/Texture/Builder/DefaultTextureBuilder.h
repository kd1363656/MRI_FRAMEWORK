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

		static constexpr std::wstring_view k_defaultBaseColorTextureFilePath = L"DefaultTextureBaseColor";
		static constexpr std::wstring_view k_defaultNormalTextureFilePath	 = L"DefaultNormal";

		static constexpr std::size_t k_defaultTextureWidth     = 1ULL;
		static constexpr std::size_t k_defaultTextureHeight    = 1ULL;
		static constexpr std::size_t k_defaultTextureArraySize = 1ULL;
		static constexpr std::size_t k_defaultTextureMIPLevels = 1ULL;

		static constexpr std::size_t k_defaultTextureMIPIndex   = 0ULL;
		static constexpr std::size_t k_defaultTextureItemIndex  = 0ULL;
		static constexpr std::size_t k_defaultTextureSliceIndex = 0ULL;

		static constexpr std::uint8_t k_defaultTextureWhiteChannel = 255U;
		static constexpr std::uint8_t k_defaultTextureAlphaChannel = 255U;
		
		static constexpr std::uint8_t k_defaultTextureNormalXChannel = 128U;
		static constexpr std::uint8_t k_defaultTextureNormalYChannel = 128U;
		static constexpr std::uint8_t k_defaultTextureNormalZChannel = 255U;
	};
}