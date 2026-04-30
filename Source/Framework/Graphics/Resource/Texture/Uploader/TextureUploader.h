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
						   const GPUMemoryAllocator&				a_gpuMemoryAllocator,
						   const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap,
						         UploadSystem&					    a_uploadSystem,
						         Struct::TextureRecord&			    a_textureRecord) const;

	private:

		bool CreateTextureResource(const DirectX::TexMetadata&                   a_texMetadata,
								   const Device&			                     a_device,
								   const GPUMemoryAllocator&					 a_gpuMemoryAllocator,
										 TypeAlias::ComPtr<ID3D12Resource2>&     a_textureResource, 
										 TypeAlias::ComPtr<D3D12MA::Allocation>& a_allocation) const;

		bool UploadTextureSubresources(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
									   const DirectX::ScratchImage&				 a_scratchImage,
									   const Device&							 a_device,
											 UploadSystem&						 a_uploadSystem) const;

		bool CreateTextureSRV(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource,
							  const DirectX::TexMetadata&               a_texMetadata,
							  const UINT	                            a_srvIndex,
							  const Device&								a_device,
							  const DescriptorPool<SRVDescriptorHeap>&  a_srvDescriptorHeap) const;

		static constexpr UINT64 k_uploadBufferBeginOffset = 0ULL;


		static constexpr UINT k_mostDetailedMIP = 0U;
		static constexpr UINT k_planeSlice		= 0U;

		static constexpr UINT k_firstArraySlice = 0U;

		static constexpr FLOAT k_resourceMINLODClamp = 0.0F;

		static constexpr std::size_t k_emptyScratchImageCount     = 0ULL;
		static constexpr std::size_t k_texture2DArrayMINArraySize = 2ULL;
	};
}