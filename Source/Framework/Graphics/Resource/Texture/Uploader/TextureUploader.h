#pragma once

namespace FWK::Graphics
{
	class TextureUploader final
	{
	public:

		 TextureUploader() = default;
		~TextureUploader() = default;

		bool UploadTexture(const DirectX::ScratchImage&			    a_scratchImage,
						   const DirectX::TexMetadata&				a_texMetadata,
						   const Device&						    a_device,
						         DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap,
						         UploadSystem&					    a_uploadSystem,
						         Struct::TextureRecord&			    a_textureRecord);

	private:

		bool CreateTextureResource    (const DirectX::TexMetadata& a_texMetadata, const Device& a_device, TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource);

		bool UploadTextureSubresources(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
									   const DirectX::ScratchImage&				 a_scratchImage,
									   const Device								 a_device,
											 UploadSystem&						 a_uploadSystem) const;

		bool CreateTextureSRV(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
							  const DirectX::TexMetadata&               a_texMetadata,
							  const UINT	                            a_srvIndex,
							  const Device&								a_device,
									DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap);

		static constexpr UINT64 k_uploadBufferStartOffset = 0ULL;

		static constexpr UINT k_textureResourceDescCount = 1U;
		static constexpr UINT k_firstSubresourceIndex    = 1U;

		static constexpr UINT k_mostDetailedMIP = 0U;
		static constexpr UINT k_planeSlice		= 0U;

		static constexpr FLOAT k_resourceMINLODClamp = 0.0F;
	};
}