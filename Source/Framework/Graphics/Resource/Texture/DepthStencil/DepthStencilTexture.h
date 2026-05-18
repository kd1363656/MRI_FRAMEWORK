#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture final
	{
	public:
	
		 DepthStencilTexture() = default;
		~DepthStencilTexture() = default;
	
		bool Create(const Graphics::Device&   a_device,
					const GPUMemoryAllocator& a_gpuMemoryAllocator,
					const UINT a_width,
					const UINT a_height,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);
	
		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALDSVStorageID() const { return m_dsvStorageID; }

	private:
	
		Struct::GPUResource m_gpuResource = {};

		TypeAlias::StorageID m_dsvStorageID = Constant::k_invalidStorageID;
	};
}